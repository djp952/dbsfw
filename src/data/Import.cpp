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
// import_artwork (local)
//
// Imports the artwork table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_artwork(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// artworkid | cardid | format | height | width | image
	auto sql = L"with input(json) as (select ?1) "
		"insert into artwork select base64decode(json_extract(input.json, '$.artworkid')), base64decode(json_extract(input.json, '$.cardid')), "
		"json_extract(input.json, '$.format'), json_extract(input.json, '$.height'), json_extract(input.json, '$.width'), "
		"base64decode(json_extract(input.json, '$.image')) from input";

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

	// cardid | name | type | passcode | text
	auto sql = L"with input(json) as (select ?1) "
		"insert into card select base64decode(json_extract(input.json, '$.cardid')), json_extract(input.json, '$.name'), "
		"json_extract(input.json, '$.type'), json_extract(input.json, '$.passcode'), json_extract(input.json, '$.text') from input";

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
// import_defaultartwork (local)
//
// Imports the defaultartwork table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_defaultartwork(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | artworkid
	auto sql = L"with input(json) as (select ?1) "
		"insert into defaultartwork select base64decode(json_extract(input.json, '$.cardid')), base64decode(json_extract(input.json, '$.artworkid')) "
		"from input";

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
// import_monster (local)
//
// Imports the monster table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_monster(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | attribute | level | type | attack | defense | normal | effect | fusion | ritual | toon | union | spirit | gemini
	auto sql = L"with input(json) as (select ?1) "
		"insert into monster select base64decode(json_extract(input.json, '$.cardid')), json_extract(input.json, '$.attribute'), "
		"json_extract(input.json, '$.level'),   json_extract(input.json, '$.type'),   json_extract(input.json, '$.attack'), "
		"json_extract(input.json, '$.defense'), json_extract(input.json, '$.normal'), json_extract(input.json, '$.effect'), "
		"json_extract(input.json, '$.fusion'),  json_extract(input.json, '$.ritual'), json_extract(input.json, '$.toon'), "
		"json_extract(input.json, '$.union'),   json_extract(input.json, '$.spirit'), json_extract(input.json, '$.gemini') "
		"from input";

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
// import_print (local)
//
// Imports the print table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_print(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// printid | cardid | seriesid | artworkid | code | language | number | rarity | limitededition | releasedate
	auto sql = L"with input(json) as (select ?1) "
		"insert into print select base64decode(json_extract(input.json, '$.printid')), base64decode(json_extract(input.json, '$.cardid')), "
		"base64decode(json_extract(input.json, '$.seriesid')), base64decode(json_extract(input.json, '$.artworkid')), json_extract(input.json, '$.code'), "
		"json_extract(input.json, '$.language'), json_extract(input.json, '$.number'), json_extract(input.json, '$.rarity'), "
		"json_extract(input.json, '$.limitededition'), json_extract(input.json, '$.releasedate') from input";

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
// import_restriction (local)
//
// Imports the restriction table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_restriction(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// restrictionlistid | cardid | restriction
	auto sql = L"with input(json) as (select ?1) "
		"insert into restriction select base64decode(json_extract(json.value, '$.restrictionlistid')), base64decode(json_extract(json.value, '$.cardid')), "
		"json_extract(json.value, '$.restriction') from input, json_each(input.json) as json";

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
// import_restrictionlist (local)
//
// Imports the restrictionlist table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_restrictionlist(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// restrictionlistid | effective
	auto sql = L"with input(json) as (select ?1) "
		"insert into restrictionlist select base64decode(json_extract(input.json, '$.restrictionlistid')), "
		"json_extract(input.json, '$.effective') from input";

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
// import_ruling (local)
//
// Imports the ruling table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_ruling(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | sequence | ruling
	auto sql = L"with input(json) as (select ?1) "
		"insert into ruling select base64decode(json_extract(json.value, '$.cardid')), json_extract(json.value, '$.sequence'), "
		"json_extract(json.value, '$.ruling') from input, json_each(input.json) as json";

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
// import_series (local)
//
// Imports the series table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_series(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// seriesid | code | name | boosterpack | releasedate
	auto sql = L"with input(json) as (select ?1) "
		"insert into series select base64decode(json_extract(input.json, '$.seriesid')), json_extract(input.json, '$.code'), "
		"json_extract(input.json, '$.name'), json_extract(input.json, '$.boosterpack'), json_extract(input.json, '$.releasedate') from input";

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
// import_spell (local)
//
// Imports the spell table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_spell(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | normal | continuous | equip | field | quickplay | ritual
	auto sql = L"with input(json) as (select ?1) "
		"insert into spell select base64decode(json_extract(input.json, '$.cardid')), json_extract(input.json, '$.normal'), "
		"json_extract(input.json, '$.continuous'), json_extract(input.json, '$.equip'), json_extract(input.json, '$.field'), "
		"json_extract(input.json, '$.quickplay'),  json_extract(input.json, '$.ritual') from input";

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
// import_trap (local)
//
// Imports the trap table 
//
// Arguments:
//
//	handle		- Database instance handle
//	path		- Path to the import files

static void import_trap(SQLiteSafeHandle^ handle, String^ path)
{
	CLRASSERT(CLRISNOTNULL(handle));
	CLRASSERT(CLRISNOTNULL(path));

	SQLiteSafeHandle::Reference instance(handle);
	sqlite3_stmt* statement = nullptr;

	// cardid | normal | continuous | counter
	auto sql = L"with input(json) as (select ?1) "
		"insert into trap select base64decode(json_extract(input.json, '$.cardid')), json_extract(input.json, '$.normal'), "
		"json_extract(input.json, '$.continuous'), json_extract(input.json, '$.counter') from input";

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
		
		// MONSTER
		//
		String^ monsterpath = Path::Combine(path, "monster");
		if(!try_create_directory(monsterpath)) throw gcnew Exception("Unable to access monster import directory");
		import_monster(handle, monsterpath);
		
		// SPELL
		//
		String^ spellpath = Path::Combine(path, "spell");
		if(!try_create_directory(spellpath)) throw gcnew Exception("Unable to access spell import directory");
		import_spell(handle, spellpath);
		
		// TRAP
		//
		String^ trappath = Path::Combine(path, "trap");
		if(!try_create_directory(trappath)) throw gcnew Exception("Unable to access trap import directory");
		import_trap(handle, trappath);
		
		// ARTWORK
		//
		String^ artworkpath = Path::Combine(path, "artwork");
		if(!try_create_directory(artworkpath)) throw gcnew Exception("Unable to access artwork import directory");
		import_artwork(handle, artworkpath);
		
		// DEFAULTARTWORK
		//
		String^ defaultartworkpath = Path::Combine(path, "defaultartwork");
		if(!try_create_directory(defaultartworkpath)) throw gcnew Exception("Unable to access defaultartwork import directory");
		import_defaultartwork(handle, defaultartworkpath);
		
		// SERIES
		//
		String^ seriespath = Path::Combine(path, "series");
		if(!try_create_directory(seriespath)) throw gcnew Exception("Unable to access series import directory");
		import_series(handle, seriespath);
		
		// PRINT
		//
		String^ printpath = Path::Combine(path, "print");
		if(!try_create_directory(printpath)) throw gcnew Exception("Unable to access print import directory");
		import_print(handle, printpath);
		
		// RESTRICTIONLIST
		//
		String^ restrictionlistpath = Path::Combine(path, "restrictionlist");
		if(!try_create_directory(restrictionlistpath)) throw gcnew Exception("Unable to access restrictionlist import directory");
		import_restrictionlist(handle, restrictionlistpath);
		
		// RESTRICTION
		//
		String^ restrictionpath = Path::Combine(path, "restriction");
		if(!try_create_directory(restrictionpath)) throw gcnew Exception("Unable to access restriction import directory");
		import_restriction(handle, restrictionpath);
		
		// RULING
		//
		String^ rulingpath = Path::Combine(path, "ruling");
		if(!try_create_directory(rulingpath)) throw gcnew Exception("Unable to access ruling import directory");
		import_ruling(handle, rulingpath);

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

} // zuki::dbsfw::data

#pragma warning(pop)
