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

#include "Database.h"

#include "SQLiteException.h"

using namespace System::IO;

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// export_card (local)
//
// Exports the card tables
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

	auto sql = LR"(
		select card.cardid, prettyjson(json_object(
			'cardid', card.cardid, 
			'type', card.type, 
			'color', card.color, 
			'rarity', card.rarity,
			'detail',
			(
				with detail(cardid, json) as
				(
				select detail.cardid, json_object('side', detail.side, 'language', detail.language, 'name', detail.name, 'cost', detail.cost, 
				  'specifiedcost', detail.specifiedcost, 'power', detail.power, 'combopower', detail.combopower, 'traits', detail.traits, 'effect', detail.effect) 
				from carddetail as detail where detail.cardid = card.cardid
				order by detail.language asc, detail.side desc
				)
				select case when detail.json is null then null else json_group_array(json(detail.json)) end from detail	
			),
			'faq',
			(
				with faq(cardid, json) as
				(
				select faq.cardid, json_object('faqid', faq.faqid, 'language', faq.language, 'question', faq.question, 'answer', faq.answer, 'related', 
				  case when related.relatedcardid is null then null else json_group_array(related.relatedcardid) end)
				from cardfaq as faq left outer join cardfaqrelated as related on faq.cardid = related.cardid and faq.faqid = related.faqid and faq.language = related.language
				where faq.cardid = card.cardid
				group by faq.cardid, faq.faqid, faq.language
				order by faq.language asc, faq.faqid asc
				)
				select case when faq.json is null then null else json_group_array(json(faq.json)) end from faq
			),
			'image',
			(
				with image(cardid, json) as
				(
				select image.cardid, json_object('side', image.side, 'language', image.language, 'format', image.format, 'image', base64encode(image.image))
				from cardimage as image where image.cardid = card.cardid
				order by image.language asc, image.side desc
				)
				select case when image.json is null then null else json_group_array(json(image.json)) end from image
			)
		)) from card
	)";

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
}

//---------------------------------------------------------------------------

}

#pragma warning(pop)
