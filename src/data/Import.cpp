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
// execute_non_query (local)
//
// Executes a database query and returns the number of rows affected
//
// Arguments:
//
//	handle		- Database instance handle
//	sql			- SQL query to execute

static int execute_non_query(SQLiteSafeHandle^ handle, wchar_t const* sql)
{
	int changes = 0;

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	if(CLRISNULL(handle)) throw gcnew ArgumentNullException("handle");
	if(sql == nullptr) throw gcnew ArgumentNullException("sql");

	// Prepare the statement
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query; ignore any rows that are returned
		do result = sqlite3_step(statement);
		while(result == SQLITE_ROW);

		// The final result from sqlite3_step should be SQLITE_DONE
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

		// Record the number of changes made by the statement
		changes = sqlite3_changes(instance);
	}

	finally { sqlite3_finalize(statement); }

	return changes;
}

//---------------------------------------------------------------------------
// import_card (local)
//
// Imports the card table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_card(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | type | color | rarity
	auto sql = L"with input(value) as (select ?1) "
		"insert into card select json_extract(input.value, '$.cardid'), json_extract(input.value, '$.type'), "
		"json_extract(input.value, '$.color'), json_extract(input.value, '$.rarity') from input";

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		for each(String^ importfile in Directory::GetFiles(path)) {

			// Read the JSON from the input file and pin it
			String^ json = File::ReadAllText(importfile);
			pin_ptr<wchar_t const> pinjson = PtrToStringChars(json);

			// Bind the query parameter(s)
			result = sqlite3_bind_text16(statement, 1, pinjson, -1, SQLITE_STATIC);
			if(result != SQLITE_OK) throw gcnew SQLiteException(result);

			// Execute the query; no rows are expected to be returned
			result = sqlite3_step(statement);
			if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

			// Reset the prepared statement so that it can be executed again
			result = sqlite3_clear_bindings(statement);
			if(result == SQLITE_OK) result = sqlite3_reset(statement);
			if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
		}
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// import_carddetail (local)
//
// Imports the carddetail table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_carddetail(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | side | language | name | cost | specifiedcost | power | combopower | traits | effect
	auto sql = L"with input(value) as (select ?1) "
		"insert into carddetail select json_extract(input.value, '$.cardid'), "
		"json_extract(detail.value, '$.side'), json_extract(detail.value, '$.language'), json_extract(detail.value, '$.name'), "
		"json_extract(detail.value, '$.cost'), json_extract(detail.value, '$.specifiedcost'), json_extract(detail.value, '$.power'), "
		"json_extract(detail.value, '$.combopower'), json_extract(detail.value, '$.traits'), json_extract(detail.value, '$.effect') "
		"from input, json_each(input.value, '$.detail') as detail "
		"where json_extract(input.value, '$.detail') is not null";

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		for each(String ^ importfile in Directory::GetFiles(path)) {

			// Read the JSON from the input file and pin it
			String^ json = File::ReadAllText(importfile);
			pin_ptr<wchar_t const> pinjson = PtrToStringChars(json);

			// Bind the query parameter(s)
			result = sqlite3_bind_text16(statement, 1, pinjson, -1, SQLITE_STATIC);
			if(result != SQLITE_OK) throw gcnew SQLiteException(result);

			// Execute the query; no rows are expected to be returned
			result = sqlite3_step(statement);
			if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

			// Reset the prepared statement so that it can be executed again
			result = sqlite3_clear_bindings(statement);
			if(result == SQLITE_OK) result = sqlite3_reset(statement);
			if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
		}
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// import_cardfaq (local)
//
// Imports the cardfaq table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_cardfaq(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | faqid | language | question | answer
	auto sql = L"with input(value) as (select ?1) "
		"insert into cardfaq select json_extract(input.value, '$.cardid'), "
		"json_extract(faq.value, '$.faqid'), json_extract(faq.value, '$.language'), json_extract(faq.value, '$.question'), "
		"json_extract(faq.value, '$.answer') "
		"from input, json_each(input.value, '$.faq') as faq "
		"where json_extract(input.value, '$.faq') is not null";

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		for each(String ^ importfile in Directory::GetFiles(path)) {

			// Read the JSON from the input file and pin it
			String^ json = File::ReadAllText(importfile);
			pin_ptr<wchar_t const> pinjson = PtrToStringChars(json);

			// Bind the query parameter(s)
			result = sqlite3_bind_text16(statement, 1, pinjson, -1, SQLITE_STATIC);
			if(result != SQLITE_OK) throw gcnew SQLiteException(result);

			// Execute the query; no rows are expected to be returned
			result = sqlite3_step(statement);
			if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

			// Reset the prepared statement so that it can be executed again
			result = sqlite3_clear_bindings(statement);
			if(result == SQLITE_OK) result = sqlite3_reset(statement);
			if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
		}
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// import_cardfaqrelated (local)
//
// Imports the cardfaqrelated table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_cardfaqrelated(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | faqid | language | relatedcardid
	auto sql = L"with input(value) as (select ?1) "
		"insert into cardfaqrelated select json_extract(input.value, '$.cardid'), "
		"json_extract(faq.value, '$.faqid'), json_extract(faq.value, '$.language'), related.value "
		"from input, json_each(input.value, '$.faq') as faq, json_each(faq.value, '$.related') as related "
		"where json_extract(faq.value, '$.related') is not null";

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		for each(String ^ importfile in Directory::GetFiles(path)) {

			// Read the JSON from the input file and pin it
			String^ json = File::ReadAllText(importfile);
			pin_ptr<wchar_t const> pinjson = PtrToStringChars(json);

			// Bind the query parameter(s)
			result = sqlite3_bind_text16(statement, 1, pinjson, -1, SQLITE_STATIC);
			if(result != SQLITE_OK) throw gcnew SQLiteException(result);

			// Execute the query; no rows are expected to be returned
			result = sqlite3_step(statement);
			if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

			// Reset the prepared statement so that it can be executed again
			result = sqlite3_clear_bindings(statement);
			if(result == SQLITE_OK) result = sqlite3_reset(statement);
			if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
		}
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// import_cardimage (local)
//
// Imports the cardimage table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_cardimage(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | side | language | format | image
	auto sql = L"with input(value) as (select ?1) "
		"insert into cardimage select json_extract(input.value, '$.cardid'), "
		"json_extract(image.value, '$.side'), json_extract(image.value, '$.language'), json_extract(image.value, '$.format'), "
		"base64decode(json_extract(image.value, '$.image')) "
		"from input, json_each(input.value, '$.image') as image "
		"where json_extract(input.value, '$.image') is not null";

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		for each(String ^ importfile in Directory::GetFiles(path)) {

			// Read the JSON from the input file and pin it
			String^ json = File::ReadAllText(importfile);
			pin_ptr<wchar_t const> pinjson = PtrToStringChars(json);

			// Bind the query parameter(s)
			result = sqlite3_bind_text16(statement, 1, pinjson, -1, SQLITE_STATIC);
			if(result != SQLITE_OK) throw gcnew SQLiteException(result);

			// Execute the query; no rows are expected to be returned
			result = sqlite3_step(statement);
			if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

			// Reset the prepared statement so that it can be executed again
			result = sqlite3_clear_bindings(statement);
			if(result == SQLITE_OK) result = sqlite3_reset(statement);
			if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
		}
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
// Database::Import (static)
//
// Creates a new database instance via import
//
// Arguments:
//
//	path		- Path to the import files created via Export()
//	output		- Path to the output database file

Database^ Database::Import(String^ path, String^ outputfile)
{
	sqlite3* instance = nullptr;			// SQLite instance handle

	if(CLRISNULL(path)) throw gcnew ArgumentNullException("path");
	if(CLRISNULL(outputfile)) throw gcnew ArgumentNullException("outputfile");

	// Canonicalize the paths to prevent traversal
	path = Path::GetFullPath(path);
	outputfile = Path::GetFullPath(outputfile);

	// Ensure the import directory exists
	if(!Directory::Exists(path)) throw gcnew Exception("Unable to access import path");

	// Ensure the output directory exists
	String^ outdir = Path::GetDirectoryName(outputfile);
	if(!try_create_directory(outdir)) throw gcnew Exception("Unable to create output directory");

	// Delete any existing output file
	if(File::Exists(outputfile)) File::Delete(outputfile);

	// Attempt to create a new the database at the specified path
	// (sqlite3_open16() implies SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE)
	pin_ptr<wchar_t const> pinoutputfile = PtrToStringChars(outputfile);
	int result = sqlite3_open16(pinoutputfile, &instance);
	if(result != SQLITE_OK) {

		if(instance != nullptr) sqlite3_close(instance);
		throw gcnew SQLiteException(result);
	}

	// Create the safe handle wrapper around the sqlite3*
	SQLiteSafeHandle^ handle = gcnew SQLiteSafeHandle(std::move(instance));
	CLRASSERT(instance == nullptr);

	// Initialize the database instance
	InitializeInstance(handle);

	try {

		// Begin a transaction to improve insert performance
		execute_non_query(handle, L"begin immediate transaction");

		// CARD
		//
		String^ cardpath = Path::Combine(path, "card");
		if(!Directory::Exists(cardpath)) throw gcnew Exception("Unable to access card import directory");

		import_card(handle, cardpath);
		import_carddetail(handle, cardpath);
		import_cardfaq(handle, cardpath);
		import_cardfaqrelated(handle, cardpath);
		import_cardimage(handle, cardpath);
		
		// Commit the transaction
		execute_non_query(handle, L"commit transaction");

		// Create and Vacuum the database instance
		Database^ database = gcnew Database(handle);
		database->Vacuum();

		return database;
	}

	catch(Exception^) {
		
		// Roll back the transaction
		execute_non_query(handle, L"rollback transaction");

		delete handle;				// Delete the safe handle
		File::Delete(outputfile);	// Delete the invalid output file
		throw;
	}
}

//---------------------------------------------------------------------------

}

#pragma warning(pop)
