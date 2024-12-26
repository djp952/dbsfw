//---------------------------------------------------------------------------
// Copyright (c) 2024-2025 Michael G. Brehm
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

#include "Database.h"

#include "SQLiteException.h"

using namespace System::IO;

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// export_artwork (local)
//
// Exports the artwork table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path on which to export the table data

static void export_artwork(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// artworkid | cardid | format | height | width | image
	auto sql = L"select uuidstr(artworkid), prettyjson(json_object('artworkid', base64encode(artworkid), 'cardid', base64encode(cardid), "
		"'format', format, 'height', height, 'width', width, 'image', base64encode(image))) from artwork";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// artworkid
			wchar_t const* artworkid = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 0));
			if(artworkid != nullptr) {

				String^ jsonfile = Path::Combine(path, gcnew String(artworkid) + ".json");
				wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
				File::WriteAllText(jsonfile, gcnew String(json));
			}

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// export_card (local)
//
// Exports the card table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path on which to export the table data

static void export_card(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | name | type | passcode | text
	auto sql = L"select uuidstr(cardid), prettyjson(json_object('cardid', base64encode(cardid), 'name', name, "
		"'type', type, 'passcode', passcode, 'text', text)) from card";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// cardid
			wchar_t const* cardid = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 0));
			if(cardid != nullptr) {

				String^ jsonfile = Path::Combine(path, gcnew String(cardid) + ".json");
				wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
				File::WriteAllText(jsonfile, gcnew String(json));
			}

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// export_defaultartwork (local)
//
// Exports the defaultartwork table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path on which to export the table data

static void export_defaultartwork(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | artworkid
	auto sql = L"select uuidstr(cardid), prettyjson(json_object('cardid', base64encode(cardid), "
		"'artworkid', base64encode(artworkid))) from defaultartwork";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// cardid
			wchar_t const* cardid = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 0));
			if(cardid != nullptr) {

				String^ jsonfile = Path::Combine(path, gcnew String(cardid) + ".json");
				wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
				File::WriteAllText(jsonfile, gcnew String(json));
			}

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// export_monster (local)
//
// Exports the monster table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path on which to export the table data

static void export_monster(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | attribute | level | type | attack | defense | normal | effect | fusion | ritual | toon | union | spirit | gemini
	auto sql = L"select uuidstr(cardid), prettyjson(json_object('cardid', base64encode(cardid), 'attribute', attribute, "
		"'level', level, 'type', type, 'attack', attack, 'defense', defense, 'normal', normal, 'effect', effect, "
		"'fusion', fusion, 'ritual', ritual, 'toon', toon, 'union', [union], 'spirit', spirit, 'gemini', gemini)) from monster";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// cardid
			wchar_t const* cardid = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 0));
			if(cardid != nullptr) {

				String^ jsonfile = Path::Combine(path, gcnew String(cardid) + ".json");
				wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
				File::WriteAllText(jsonfile, gcnew String(json));
			}

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// export_print (local)
//
// Exports the print table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path on which to export the table data

static void export_print(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// printid | cardid | seriesid | artworkid | code | language | number | rarity | limitededition | releasedate
	auto sql = L"select uuidstr(printid), prettyjson(json_object('printid', base64encode(printid), 'cardid', base64encode(cardid), "
		"'seriesid', base64encode(seriesid), 'artworkid', base64encode(artworkid), 'code', code, 'language', language, "
		"'number', number, 'rarity', rarity, 'limitededition', limitededition, 'releasedate', releasedate)) from print";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// printid
			wchar_t const* printid = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 0));
			if(printid != nullptr) {

				String^ jsonfile = Path::Combine(path, gcnew String(printid) + ".json");
				wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
				File::WriteAllText(jsonfile, gcnew String(json));
			}

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// export_restriction (local)
//
// Exports the restriction table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path on which to export the table data

static void export_restriction(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// restrictionlistid | cardid | restriction
	auto sql = L"select uuidstr(restrictionlistid), prettyjson(json_group_array(json_object('restrictionlistid', base64encode(restrictionlistid), "
		"'cardid', base64encode(cardid), 'restriction', restriction))) from restriction group by restrictionlistid";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// restrictionlistid
			wchar_t const* restrictionlistid = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 0));
			if(restrictionlistid != nullptr) {

				String^ jsonfile = Path::Combine(path, gcnew String(restrictionlistid) + ".json");
				wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
				File::WriteAllText(jsonfile, gcnew String(json));
			}

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// export_restrictionlist (local)
//
// Exports the restrictionlist table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path on which to export the table data

static void export_restrictionlist(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// restrictionlistid | effective
	auto sql = L"select uuidstr(restrictionlistid), prettyjson(json_object('restrictionlistid', base64encode(restrictionlistid), "
		"'effective', effective)) from restrictionlist";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// restrictionlistid
			wchar_t const* restrictionlistid = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 0));
			if(restrictionlistid != nullptr) {

				String^ jsonfile = Path::Combine(path, gcnew String(restrictionlistid) + ".json");
				wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
				File::WriteAllText(jsonfile, gcnew String(json));
			}

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// export_ruling (local)
//
// Exports the restriction table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path on which to export the table data

static void export_ruling(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | sequence | ruling
	auto sql = L"select uuidstr(cardid), prettyjson(json_group_array(json_object('cardid', base64encode(cardid), "
		"'sequence', sequence, 'ruling', ruling))) from ruling group by cardid";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// cardid
			wchar_t const* cardid = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 0));
			if(cardid != nullptr) {

				String^ jsonfile = Path::Combine(path, gcnew String(cardid) + ".json");
				wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
				File::WriteAllText(jsonfile, gcnew String(json));
			}

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// export_series (local)
//
// Exports the series table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path on which to export the table data

static void export_series(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// seriesid | code | name | boosterpack | releasedate
	auto sql = L"select uuidstr(seriesid), prettyjson(json_object('seriesid', base64encode(seriesid), "
		"'code', code, 'name', name, 'boosterpack', boosterpack, 'releasedate', releasedate)) from series";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// seriesid
			wchar_t const* seriesid = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 0));
			if(seriesid != nullptr) {

				String^ jsonfile = Path::Combine(path, gcnew String(seriesid) + ".json");
				wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
				File::WriteAllText(jsonfile, gcnew String(json));
			}

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// export_spell (local)
//
// Exports the spell table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path on which to export the table data

static void export_spell(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | normal | continuous | equip | field | quickplay | ritual
	auto sql = L"select uuidstr(cardid), prettyjson(json_object('cardid', base64encode(cardid), 'normal', normal, "
		"'continuous', continuous, 'equip', equip, 'field', field, 'quickplay', quickplay, 'ritual', ritual)) from spell";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// cardid
			wchar_t const* cardid = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 0));
			if(cardid != nullptr) {

				String^ jsonfile = Path::Combine(path, gcnew String(cardid) + ".json");
				wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
				File::WriteAllText(jsonfile, gcnew String(json));
			}

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// export_trap (local)
//
// Exports the trap table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path on which to export the table data

static void export_trap(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | normal | continuous | counter
	auto sql = L"select uuidstr(cardid), prettyjson(json_object('cardid', base64encode(cardid), 'normal', normal, "
		"'continuous', continuous, 'counter', counter)) from trap";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// cardid
			wchar_t const* cardid = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 0));
			if(cardid != nullptr) {

				String^ jsonfile = Path::Combine(path, gcnew String(cardid) + ".json");
				wchar_t const* json = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
				File::WriteAllText(jsonfile, gcnew String(json));
			}

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// try_create_directory (local)
//
// Attempts to create a directory if it does not exist
//
// Arguments:
//
//	path		- Directory to be created

static bool try_create_directory(String^ path)
{
	CLRASSERT(CLRISNOTNULL(path));
	if(CLRISNULL(path)) throw gcnew ArgumentNullException("path");

	if(!Directory::Exists(path)) {

		try { Directory::CreateDirectory(path); }
		catch(Exception^) { return false; }
	}

	return true;
}

//---------------------------------------------------------------------------
// Database::Export
//
// Exports the database into flat files for storage
//
// Arguments:
//
//	path		- Base path for the export operation

void Database::Export(String^ path)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(path)) throw gcnew ArgumentNullException("path");

	// Check that the output path exists and try to create it if not
	path = Path::GetFullPath(path);
	if(!try_create_directory(path)) throw gcnew Exception("Unable to create specified export directory");

	// CARD
	//
	String^ cardpath = Path::Combine(path, "card");
	if(!try_create_directory(cardpath)) throw gcnew Exception("Unable to create card export directory");
	export_card(m_handle, cardpath);

	// MONSTER
	//
	String^ monsterpath = Path::Combine(path, "monster");
	if(!try_create_directory(monsterpath)) throw gcnew Exception("Unable to create monster export directory");
	export_monster(m_handle, monsterpath);

	// SPELL
	//
	String^ spellpath = Path::Combine(path, "spell");
	if(!try_create_directory(spellpath)) throw gcnew Exception("Unable to create spell export directory");
	export_spell(m_handle, spellpath);

	// TRAP
	//
	String^ trappath = Path::Combine(path, "trap");
	if(!try_create_directory(trappath)) throw gcnew Exception("Unable to create trap export directory");
	export_trap(m_handle, trappath);

	// ARTWORK
	//
	String^ artworkpath = Path::Combine(path, "artwork");
	if(!try_create_directory(artworkpath)) throw gcnew Exception("Unable to create artwork export directory");
	export_artwork(m_handle, artworkpath);

	// DEFAULTARTWORK
	//
	String^ defaultartworkpath = Path::Combine(path, "defaultartwork");
	if(!try_create_directory(defaultartworkpath)) throw gcnew Exception("Unable to create defaultartwork export directory");
	export_defaultartwork(m_handle, defaultartworkpath);

	// SERIES
	//
	String^ seriespath = Path::Combine(path, "series");
	if(!try_create_directory(seriespath)) throw gcnew Exception("Unable to create series export directory");
	export_series(m_handle, seriespath);

	// PRINT
	//
	String^ printpath = Path::Combine(path, "print");
	if(!try_create_directory(printpath)) throw gcnew Exception("Unable to create print export directory");
	export_print(m_handle, printpath);

	// RESTRICTIONLIST
	//
	String^ restrictionlistpath = Path::Combine(path, "restrictionlist");
	if(!try_create_directory(restrictionlistpath)) throw gcnew Exception("Unable to create restrictionlist export directory");
	export_restrictionlist(m_handle, restrictionlistpath);

	// RESTRICTION
	//
	String^ restrictionpath = Path::Combine(path, "restriction");
	if(!try_create_directory(restrictionpath)) throw gcnew Exception("Unable to create restriction export directory");
	export_restriction(m_handle, restrictionpath);

	// RULING
	//
	String^ rulingpath = Path::Combine(path, "ruling");
	if(!try_create_directory(rulingpath)) throw gcnew Exception("Unable to create ruling export directory");
	export_ruling(m_handle, rulingpath);
}

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)
