//---------------------------------------------------------------------------
// Copyright (c) 2025 Michael G. Brehm
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------

#include "stdafx.h"

#include <assert.h>
#include <rpc.h>
#include <sqlite3ext.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "align.h"
#include "CardType.h"

// RapidJSON tries to use intrinsics that cause warnings when compiled with
// CLR support; performance isn't necessary here so get rid of them
#pragma push_macro("_MSC_VER")
#undef _MSC_VER
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#pragma pop_macro("_MSC_VER")

#include "webp\decode.h"

extern "C" { SQLITE_EXTENSION_INIT1 };

using namespace zuki::dbsfw::data;

#pragma warning(push, 4)

//---------------------------------------------------------------------------
// base64decode (local)
//
// SQLite scalar function to convert a base-64 encoded string into a blob
//
// Arguments:
//
//	context		- SQLite context object
//	argc		- Number of supplied arguments
//	argv		- Argument values

static void base64decode(sqlite3_context* context, int argc, sqlite3_value** argv)
{
	if((argc != 1) || (argv[0] == nullptr)) return sqlite3_result_error(context, "invalid arguments", -1);

	DWORD cb = 0;			// Length of the decoded binary data

	// Grab a pointer to the input string in UTF-16
	wchar_t const* input = reinterpret_cast<wchar_t const*>(sqlite3_value_text16(argv[0]));
	if(input == nullptr) return sqlite3_result_null(context);

	// Determine the length of the binary data that would result from conversion
	CryptStringToBinaryW(input, 0, CRYPT_STRING_BASE64_ANY, nullptr, &cb, nullptr, nullptr);

	// Allocate the memory using sqlite3_malloc64
	LPBYTE data = reinterpret_cast<LPBYTE>(sqlite3_malloc64(cb));
	if(data == nullptr) return sqlite3_result_error(context, "unable to allocate memory", -1);

	// Convert the base-64 encoded string back into binary data
	if(!CryptStringToBinaryW(input, 0, CRYPT_STRING_BASE64_ANY, data, &cb, nullptr, nullptr)) {

		sqlite3_free(data);
		return sqlite3_result_error(context, "failed to decode binary data from base-64", -1);
	}

	return sqlite3_result_blob(context, data, static_cast<int>(cb), sqlite3_free);
}

//---------------------------------------------------------------------------
// base64encode (local)
//
// SQLite scalar function to convert a blob column into a base-64 string
//
// Arguments:
//
//	context		- SQLite context object
//	argc		- Number of supplied arguments
//	argv		- Argument values

static void base64encode(sqlite3_context* context, int argc, sqlite3_value** argv)
{
	if((argc != 1) || (argv[0] == nullptr)) return sqlite3_result_error(context, "invalid arguments", -1);

	DWORD cb = 0;			// Length, in bytes, of the base-64 encoded string

	// Get the length of the data to be encoded
	int length = sqlite3_value_bytes(argv[0]);
	if(length == 0) return sqlite3_result_null(context);

	// Determine the length of the resultant base-64 encoded string
	LPCBYTE data = reinterpret_cast<LPCBYTE>(sqlite3_value_blob(argv[0]));
	CryptBinaryToStringW(data, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &cb);

	// Allocate the memory using sqlite3_malloc64
	LPWSTR pwsz = reinterpret_cast<LPWSTR>(sqlite3_malloc64(cb * sizeof(wchar_t)));
	if(pwsz == nullptr) return sqlite3_result_error(context, "unable to allocate memory", -1);

	// Convert the binary data into the base-64 encoded string value
	if(!CryptBinaryToStringW(data, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, pwsz, &cb)) {

		sqlite3_free(pwsz);
		return sqlite3_result_error(context, "failed to encode binary data into base-64", -1);
	}
	
	return sqlite3_result_text16(context, pwsz, -1, sqlite3_free);
}

//---------------------------------------------------------------------------
// cardtype (local)
//
// SQLite scalar function to convert a card type string into a CardType
//
// Arguments:
//
//	context		- SQLite context object
//	argc		- Number of supplied arguments
//	argv		- Argument values

static void cardtype(sqlite3_context* context, int argc, sqlite3_value** argv)
{
	if((argc != 1) || (argv[0] == nullptr)) return sqlite3_result_error(context, "invalid arguments", -1);

	// Null or zero-length input string results in CardType::None
	wchar_t const* str = reinterpret_cast<wchar_t const*>(sqlite3_value_text16(argv[0]));
	if((str == nullptr) || (*str == L'\0')) return sqlite3_result_int(context, static_cast<int>(CardType::None));

	// The strings are case-sensitive and enforced by a CHECK CONSTRAINT
	if(wcscmp(str, L"LEADER") == 0) return sqlite3_result_int(context, static_cast<int>(CardType::Leader));
	else if(wcscmp(str, L"BATTLE") == 0) return sqlite3_result_int(context, static_cast<int>(CardType::Battle));
	else if(wcscmp(str, L"EXTRA") == 0) return sqlite3_result_int(context, static_cast<int>(CardType::Extra));

	// Input string was not a valid card type
	return sqlite3_result_int(context, static_cast<int>(CardType::None));
}

//---------------------------------------------------------------------------
// newid (local)
//
// SQLite scalar function to generate a UUID
//
// Arguments:
//
//	context		- SQLite context object
//	argc		- Number of supplied arguments
//	argv		- Argument values

static void newid(sqlite3_context* context, int argc, sqlite3_value** /*argv*/)
{
	UUID uuid = {};

	if(argc != 0) return sqlite3_result_error(context, "invalid argument", -1);

	// Create a new UUID using Windows RPC
	RPC_STATUS status = UuidCreate(&uuid);
	if(status != RPC_S_OK) { /* Suppress C6031 */ }

	// Return the UUID back as a 16-byte blob
	return sqlite3_result_blob(context, &uuid, sizeof(UUID), SQLITE_TRANSIENT);
}

//---------------------------------------------------------------------------
// prettyjson (local)
//
// SQLite scalar function to pretty print a JSON string
//
// Arguments:
//
//	context		- SQLite context object
//	argc		- Number of supplied arguments
//	argv		- Argument values

static void prettyjson(sqlite3_context* context, int argc, sqlite3_value** argv)
{
	if((argc != 1) || (argv[0] == nullptr)) return sqlite3_result_error(context, "invalid arguments", -1);

	// Null or zero-length input string results in null
	wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_value_text16(argv[0]));
	if((json == nullptr) || (*json == L'\0')) return sqlite3_result_null(context);

	// Pretty print the JSON using rapidjson
	rapidjson::GenericDocument<rapidjson::UTF16<>> document;
	document.Parse(json);
	rapidjson::GenericStringBuffer<rapidjson::UTF16<>> sb;
	rapidjson::PrettyWriter<rapidjson::GenericStringBuffer<rapidjson::UTF16<>>, rapidjson::UTF16<>, rapidjson::UTF16<>> writer(sb);
	writer.SetIndent(L' ', 2);
	document.Accept(writer);

	return sqlite3_result_text16(context, sb.GetString(), -1, SQLITE_TRANSIENT);
}

//---------------------------------------------------------------------------
// uuid (local)
//
// SQLite scalar function to convert a string into a UUID
//
// Arguments:
//
//	context		- SQLite context object
//	argc		- Number of supplied arguments
//	argv		- Argument values

static void uuid(sqlite3_context* context, int argc, sqlite3_value** argv)
{
	if((argc != 1) || (argv[0] == nullptr)) return sqlite3_result_error(context, "invalid arguments", -1);

	// Use managed code to parse the string to access Guid::TryParse(), which allows all the formats
	wchar_t const* inputptr = reinterpret_cast<wchar_t const*>(sqlite3_value_text16(argv[0]));
	if(inputptr != nullptr) {

		Guid uuid = Guid::Empty;
		if(Guid::TryParse(gcnew String(inputptr), uuid)) {

			// If the Guid parsed, return it as a 16-byte blob
			array<Byte>^ bytes = uuid.ToByteArray();
			pin_ptr<Byte> pinbytes = &bytes[0];
			return sqlite3_result_blob(context, &pinbytes[0], sizeof(UUID), SQLITE_TRANSIENT);
		}
	}

	return sqlite3_result_null(context);
}

//---------------------------------------------------------------------------
// uuidstr (local)
//
// SQLite scalar function to convert a binary UUID into a string
//
// Arguments:
//
//	context		- SQLite context object
//	argc		- Number of supplied arguments
//	argv		- Argument values

static void uuidstr(sqlite3_context* context, int argc, sqlite3_value** argv)
{
	if((argc != 1) || (argv[0] == nullptr)) return sqlite3_result_error(context, "invalid arguments", -1);

	// The length of the blob must match the size of a UUID
	int length = sqlite3_value_bytes(argv[0]);
	if(length == sizeof(UUID)) {

		// Use RPC to convert the UUID blob into a string (.NET "D" format)
		RPC_WSTR uuidstr = nullptr;
		RPC_STATUS status = UuidToString(reinterpret_cast<UUID const*>(sqlite3_value_blob(argv[0])), &uuidstr);
		if(status == RPC_S_OK) {

			sqlite3_result_text16(context, uuidstr, -1, SQLITE_TRANSIENT);
			RpcStringFree(&uuidstr);
			return;
		}
	}

	return sqlite3_result_null(context);
}

//---------------------------------------------------------------------------
// webpdecode (local)
//
// SQLite scalar function to convert a webp blob into another format
//
// Arguments:
//
//	context		- SQLite context object
//	argc		- Number of supplied arguments
//	argv		- Argument values

static void webpdecode(sqlite3_context* context, int argc, sqlite3_value** argv)
{
	if((argc != 1) || (argv[0] == nullptr)) return sqlite3_result_error(context, "invalid arguments", -1);

	// The argument is always treated as a blob and null results in null
	uint8_t const* blob = reinterpret_cast<uint8_t const*>(sqlite3_value_blob(argv[0]));
	if(blob == nullptr) return sqlite3_result_null(context);

	// Get the width and height of the input WebP image blob
	int width = 0, height = 0;
	int result = WebPGetInfo(blob, sqlite3_value_bytes(argv[0]), &width, &height);
	if(result == 0) return sqlite3_result_error(context, "invalid webp header", -1);

	// There are three headers - the file header, the V5 info header and three RGBQUADs
	size_t const cbheaders = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPV5HEADER) + (sizeof(RGBQUAD) * 3);

	int stride = align::up(width * 4, 4);		// 32bpp
	size_t cbdata = stride * height;			// Data size
	size_t cbfile = cbheaders + cbdata;			// File size

	// Allocate the buffer required to hold the entire file
	uint8_t* file = reinterpret_cast<uint8_t*>(sqlite3_malloc64(cbfile));
	if(file == nullptr) return sqlite3_result_error(context, "insufficient memory", -1);
	memset(file, 0, cbfile);

	// Get pointers to everything we need pointers to
	BITMAPFILEHEADER* fileheader = reinterpret_cast<BITMAPFILEHEADER*>(file);
	BITMAPV5HEADER* infoheader = reinterpret_cast<BITMAPV5HEADER*>(file + sizeof(BITMAPFILEHEADER));
	RGBQUAD* rgbquads = reinterpret_cast<RGBQUAD*>(file + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPV5HEADER));
	uint8_t* data = file + cbheaders;

	// BITMAPFILEHEADER
	fileheader->bfType = 0x4D42;
	fileheader->bfSize = static_cast<DWORD>(cbfile);
	fileheader->bfOffBits = cbheaders;

	// BITMAPV5HEADER
	infoheader->bV5Size = sizeof(BITMAPV5HEADER);
	infoheader->bV5Width = width;
	infoheader->bV5Height = -height;			// Upper-left
	infoheader->bV5Planes = 1;
	infoheader->bV5BitCount = 32;
	infoheader->bV5Compression = BI_BITFIELDS;
	infoheader->bV5SizeImage = static_cast<DWORD>(cbdata);
	infoheader->bV5XPelsPerMeter = 3780;		// 96dpi
	infoheader->bV5YPelsPerMeter = 3780;		// 96dpi
	infoheader->bV5CSType = LCS_WINDOWS_COLOR_SPACE;

	// BGRA
	infoheader->bV5RedMask = 0x00FF0000;
	infoheader->bV5GreenMask = 0x0000FF00;
	infoheader->bV5BlueMask = 0x000000FF;
	infoheader->bV5AlphaMask = 0xFF000000;

	// BGRA
	rgbquads[0].rgbBlue = 0xFF;
	rgbquads[1].rgbGreen = 0xFF;
	rgbquads[2].rgbRed = 0xFF;

	// Decode the WebP image into a 32bpp BGRA format bitmap image
	if(WebPDecodeBGRAInto(blob, sqlite3_value_bytes(argv[0]), data, cbdata, stride) == nullptr) {

		sqlite3_free(file);
		return sqlite3_result_error(context, "failed to decode webp blob into bgra format", -1);
	}

	return sqlite3_result_blob64(context, file, cbfile, sqlite3_free);
}

//---------------------------------------------------------------------------
// sqlite3_extension_init
//
// SQLite Extension Library entry point
//
// Arguments:
//
//	db		- SQLite database instance
//	errmsg	- On failure set to the error message (use sqlite3_malloc() to allocate)
//	api		- Pointer to the SQLite API functions

extern "C" int sqlite3_extension_init(sqlite3* db, char** errmsg, const sqlite3_api_routines* api)
{
	SQLITE_EXTENSION_INIT2(api);

	*errmsg = nullptr;							// Initialize [out] variable

	// base64decode function
	//
	int result = sqlite3_create_function16(db, L"base64decode", 1, SQLITE_UTF16, nullptr, base64decode, nullptr, nullptr);
	if(result != SQLITE_OK) { *errmsg = sqlite3_mprintf("Unable to register scalar function base64decode (%d)", result); return result; }

	// base64encode function
	//
	result = sqlite3_create_function16(db, L"base64encode", 1, SQLITE_UTF16, nullptr, base64encode, nullptr, nullptr);
	if(result != SQLITE_OK) { *errmsg = sqlite3_mprintf("Unable to register scalar function base64encode (%d)", result); return result; }

	// cardtype function
	//
	result = sqlite3_create_function16(db, L"cardtype", 1, SQLITE_UTF16, nullptr, cardtype, nullptr, nullptr);
	if(result != SQLITE_OK) { *errmsg = sqlite3_mprintf("Unable to register scalar function cardtype (%d)", result); return result; }

	// newid function
	//
	result = sqlite3_create_function16(db, L"newid", 0, SQLITE_UTF16, nullptr, newid, nullptr, nullptr);
	if(result != SQLITE_OK) { *errmsg = sqlite3_mprintf("Unable to register scalar function newid (%d)", result); return result; }

	// prettyjson function
	//
	result = sqlite3_create_function16(db, L"prettyjson", 1, SQLITE_UTF16, nullptr, prettyjson, nullptr, nullptr);
	if(result != SQLITE_OK) { *errmsg = sqlite3_mprintf("Unable to register scalar function prettyjson (%d)", result); return result; }

	// uuid function
	//
	result = sqlite3_create_function16(db, L"uuid", 1, SQLITE_UTF16, nullptr, uuid, nullptr, nullptr);
	if(result != SQLITE_OK) { *errmsg = sqlite3_mprintf("Unable to register scalar function uuid (%d)", result); return result; }

	// uuidstr function
	//
	result = sqlite3_create_function16(db, L"uuidstr", 1, SQLITE_UTF16, nullptr, uuidstr, nullptr, nullptr);
	if(result != SQLITE_OK) { *errmsg = sqlite3_mprintf("Unable to register scalar function uuidstr (%d)", result); return result; }

	// webpdecode function
	//
	result = sqlite3_create_function16(db, L"webpdecode", 1, SQLITE_UTF16, nullptr, webpdecode, nullptr, nullptr);
	if(result != SQLITE_OK) { *errmsg = sqlite3_mprintf("Unable to register scalar function webpdecode (%d)", result); return result; }

	return SQLITE_OK;
}

//---------------------------------------------------------------------------

#pragma warning(pop)
