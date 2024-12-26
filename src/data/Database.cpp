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

#include <string>

#include "SQLiteException.h"

using namespace System::IO;
using namespace System::Runtime::InteropServices;

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// bind_parameter (local)
//
// Used by execute_non_query to bind a String^ parameter
//
// Arguments:
//
//	statement		- SQL statement instance
//	paramindex		- Index of the parameter to bind; will be incremented
//	value			- Value to bind as the parameter

static void bind_parameter(sqlite3_stmt* statement, int& paramindex, String^ value)
{
	int					result;				// Result from binding operation

	if(CLRISNOTNULL(value)) {

		// Pin the String and specify SQLITE_TRANSIENT to have SQLite copy the string
		pin_ptr<const wchar_t> pintext = PtrToStringChars(value);
		result = sqlite3_bind_text16(statement, paramindex++, pintext, -1, SQLITE_TRANSIENT);
	}

	else result = sqlite3_bind_null(statement, paramindex++);

	if(result != SQLITE_OK) throw gcnew SQLiteException(result);
}

//---------------------------------------------------------------------------
// column_string (local)
//
// Converts a SQLite text result column into a System::String
//
// Arguments:
//
//	statement		- SQL statement instance
//	index			- Index of the result column

static String^ column_string(sqlite3_stmt* statement, int index)
{
	wchar_t const* stringptr = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, index));
	return (stringptr == nullptr) ? String::Empty : gcnew String(stringptr);
}

//---------------------------------------------------------------------------
// column_uuid (local)
//
// Converts a SQLite BLOB result column into a System::Guid
//
// Arguments:
//
//	statement		- SQL statement instance
//	index			- Index of the result column

static Guid column_uuid(sqlite3_stmt* statement, int index)
{
	int bloblen = sqlite3_column_bytes(statement, index);
	if(bloblen == 0) return Guid::Empty;
	if(bloblen != sizeof(UUID)) throw gcnew Exception("Invalid BLOB length for conversion to System::Guid");

	array<byte>^ blob = gcnew array<byte>(bloblen);
	Marshal::Copy(IntPtr(const_cast<void*>(sqlite3_column_blob(statement, index))), blob, 0, bloblen);

	return Guid(blob);
}

//---------------------------------------------------------------------------
// execute_non_query (local)
//
// Executes a database query and returns the number of rows affected
//
// Arguments:
//
//	instance		- Database instance
//	sql				- SQL query to execute
//	parameters		- Parameters to be bound to the query

template<typename... _parameters>
static int execute_non_query(sqlite3* instance, wchar_t const* sql, _parameters&&... parameters)
{
	sqlite3_stmt* statement = nullptr;
	int	paramindex = 1;

	if(instance == nullptr) throw gcnew ArgumentNullException("instance");
	if(sql == nullptr) throw gcnew ArgumentNullException("sql");

	// Suppress unreferenced local variable warning when there are no parameters to bind
	(void)paramindex;

	// Prepare the statement
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the provided query parameter(s) by unpacking the parameter pack
		int unpack[] = { 0, (static_cast<void>(bind_parameter(statement, paramindex, parameters)), 0) ... };
		(void)unpack;

		// Execute the query; ignore any rows that are returned
		do result = sqlite3_step(statement);
		while(result == SQLITE_ROW);

		// The final result from sqlite3_step should be SQLITE_DONE
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

		sqlite3_finalize(statement);

		// Return the number of changes made by the statement
		return sqlite3_changes(instance);
	}

	catch(Exception^) { sqlite3_finalize(statement); throw; }
}

//---------------------------------------------------------------------------
// execute_scalar_int (local)
//
// Executes a database query and returns a scalar integer result
//
// Arguments:
//
//	instance		- Database instance
//	sql				- SQL query to execute
//	parameters		- Parameters to be bound to the query

template<typename... _parameters>
static int execute_scalar_int(sqlite3* instance, wchar_t const* sql, _parameters&&... parameters)
{
	sqlite3_stmt* statement = nullptr;
	int	paramindex = 1;
	int	value = 0;

	if(instance == nullptr) throw gcnew ArgumentNullException("instance");
	if(sql == nullptr) throw gcnew ArgumentNullException("sql");

	// Suppress unreferenced local variable warning when there are no parameters to bind
	(void)paramindex;

	// Prepare the statement
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the provided query parameter(s) by unpacking the parameter pack
		int unpack[] = { 0, (static_cast<void>(bind_parameter(statement, paramindex, parameters)), 0) ... };
		(void)unpack;

		// Execute the query; only the first row returned will be used
		result = sqlite3_step(statement);

		if(result == SQLITE_ROW) value = sqlite3_column_int(statement, 0);
		else if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

		sqlite3_finalize(statement);

		// Return the resultant value from the scalar query
		return value;
	}

	catch(Exception^) { sqlite3_finalize(statement); throw; }
}

//---------------------------------------------------------------------------
// execute_scalar_int64 (local)
//
// Executes a database query and returns a scalar 64-bit integer result
//
// Arguments:
//
//	instance		- Database instance
//	sql				- SQL query to execute
//	parameters		- Parameters to be bound to the query

template<typename... _parameters>
static int64_t execute_scalar_int64(sqlite3* instance, wchar_t const* sql, _parameters&&... parameters)
{
	sqlite3_stmt* statement = nullptr;
	int	paramindex = 1;
	int64_t	value = 0;

	if(instance == nullptr) throw gcnew ArgumentNullException("instance");
	if(sql == nullptr) throw gcnew ArgumentNullException("sql");

	// Suppress unreferenced local variable warning when there are no parameters to bind
	(void)paramindex;

	// Prepare the statement
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the provided query parameter(s) by unpacking the parameter pack
		int unpack[] = { 0, (static_cast<void>(bind_parameter(statement, paramindex, parameters)), 0) ... };
		(void)unpack;

		// Execute the query; only the first row returned will be used
		result = sqlite3_step(statement);

		if(result == SQLITE_ROW) value = sqlite3_column_int64(statement, 0);
		else if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

		sqlite3_finalize(statement);

		// Return the resultant value from the scalar query
		return value;
	}

	catch(Exception^) { sqlite3_finalize(statement); throw; }
}

//---------------------------------------------------------------------------
// Database Static Constructor (private)
//
// Arguments:
//
//	NONE
	
static Database::Database()
{
	// Automatically register the built-in database extension library functions
	s_result = sqlite3_auto_extension(reinterpret_cast<void(*)()>(sqlite3_extension_init));
}

//---------------------------------------------------------------------------
// Database Constructor (private)
//
// Arguments:
//
//	handle		- SQLiteSafeHandle instance

Database::Database(SQLiteSafeHandle^ handle) : m_handle(handle)
{
	if(CLRISNULL(handle)) throw gcnew ArgumentNullException("handle");

	// Ensure that the static initialization completed successfully
	if(s_result != SQLITE_OK)
		throw gcnew Exception("Static initialization failed", gcnew SQLiteException(s_result));
}

//---------------------------------------------------------------------------
// Database Destructor (private)

Database::~Database()
{
	if(m_disposed) return;

	delete m_handle;					// Release the safe handle
	m_disposed = true;					// Object is now in a disposed state
}

//---------------------------------------------------------------------------
// Database::InitializeInstance (private, static)
//
// Initializes the database instance for use
//
// Arguments:
//
//	handle		- SQLiteSafeHandle instance

void Database::InitializeInstance(SQLiteSafeHandle^ handle)
{
	if(CLRISNULL(handle)) throw gcnew ArgumentNullException("handle");
	
	SQLiteSafeHandle::Reference instance(handle);

	// Set the instance to report extended error codes
	sqlite3_extended_result_codes(instance, TRUE);

	// Set a busy timeout handler for this connection
	sqlite3_busy_timeout(instance, 5000);

	// Switch the database to write-ahead logging
	execute_non_query(instance, L"pragma journal_mode=wal");

	// Switch the database to UTF-16 encoding
	execute_non_query(instance, L"pragma encoding='UTF-16'");

	// Enable foreign key constraints
	execute_non_query(instance, L"pragma foreign_keys=ON");

	// Get the database schema version
	int dbversion = execute_scalar_int(instance, L"pragma user_version");

	// SCHEMA VERSION 0 -> VERSION 1
	//
	// Original database schema
	if(dbversion == 0) {

		// table: card
		//
		// cardid(pk) | type | color | rarity
		execute_non_query(instance, L"create table card(cardid text not null, type text not null, color text not null, rarity text not null, "
			"primary key(cardid), "
			"check(type in ('LEADER', 'BATTLE', 'EXTRA')), check(color in ('Red', 'Blue', 'Green', 'Yellow', 'Black')), "
			"check(rarity in ('L', 'C', 'R', 'SR', 'SCR', 'PR')))");

		// table: carddetail
		//
		// cardid(pk|fk) | side(pk) | language(pk) | name | cost | specifiedcost | power | combopower | traits | effect
		execute_non_query(instance, L"create table carddetail(cardid text not null, side text null, language text not null, name text not null, "
			"cost integer null, specifiedcost text null, power integer null, combopower integer null, traits text null, effect text null, "
			"primary key(cardid, side, language) foreign key(cardid) references card(cardid), "
			"check(side in (null, 'FRONT', 'BACK')), check(language in('EN', 'JP')))");

		// table: cardfaq
		//
		// cardid(pk|fk) | faqid(pk) | language(pk) | question | answer
		execute_non_query(instance, L"create table cardfaq(cardid text not null, faqid text not null, language text not null, question text not null, "
			"answer text null, "
			"primary key(cardid, faqid, language) foreign key(cardid) references card(cardid), "
			"check(language in('EN', 'JP')))");

		// table: cardfaqrelated
		//
		// cardid(pk|fk) | faqid(pk|fk) | language(pk|fk) | relatedcardid (pk)
		execute_non_query(instance, L"create table cardfaqrelated(cardid text not null, faqid text null, language text not null, relatedcardid text not null, "
			"primary key(cardid, faqid, language, relatedcardid) foreign key(cardid, faqid, language) references cardfaq(cardid, faqid, language), "
			"check(language in('EN', 'JP')))");

		// table: cardimage
		//
		// cardid(pk|fk) | side(pk) | language(pk) | format | image
		execute_non_query(instance, L"create table cardimage(cardid text not null, side text null, language text not null, "
			"format text not null, image blob not null, "
			"primary key(cardid, side, language) foreign key(cardid) references card(cardid), "
			"check(side in (null, 'FRONT', 'BACK')), check(language in ('EN', 'JP')))");

		execute_non_query(instance, L"pragma user_version = 1");
		dbversion = 1;
	}

	CLRASSERT(dbversion == 1);
}

//---------------------------------------------------------------------------
// Database::Open (static)
//
// Opens an existing database file
//
// Arguments:
//
//	path		- Path on which to open the database file

Database^ Database::Open(String^ path)
{
	sqlite3* instance = nullptr;

	if(CLRISNULL(path)) throw gcnew ArgumentNullException("path");

	// Create a marshaling context to convert the String^ into an ANSI C-style string
	msclr::auto_handle<msclr::interop::marshal_context> context(gcnew msclr::interop::marshal_context());

	// Attempt to open the database on the specified path
	int result = sqlite3_open_v2(context->marshal_as<char const*>(Path::GetFullPath(path)), &instance,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
	if(result != SQLITE_OK) {

		if(instance != nullptr) sqlite3_close(instance);
		throw gcnew SQLiteException(result);
	}

	// Create the safe handle wrapper around the sqlite3*
	SQLiteSafeHandle^ handle = gcnew SQLiteSafeHandle(std::move(instance));
	CLRASSERT(instance == nullptr);

	// Initialize the database instance
	InitializeInstance(handle);

	// Delete the safe handle on a construction failure
	try { return gcnew Database(handle); }
	catch(Exception^) { delete handle; throw; }
}

//---------------------------------------------------------------------------
// Database::Vacuum
//
// Vacuums the database
//
// Arguments:
//
//	NONE

int64_t Database::Vacuum(void)
{
	int64_t unused;
	return Vacuum(unused);
}

//---------------------------------------------------------------------------
// Database::Vacuum
//
// Vacuums the database
//
// Arguments:
//
//	oldsize		- Size of the database prior to vacuum

int64_t Database::Vacuum([OutAttribute] int64_t% oldsize)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	SQLiteSafeHandle::Reference instance(m_handle);

	// Get the size of the database prior to vacuuming
	int pagesize = execute_scalar_int(instance, L"pragma page_size");
	int64_t pagecount = execute_scalar_int64(instance, L"pragma page_count");
	oldsize = pagecount * pagesize;

	execute_non_query(instance, L"vacuum");

	// Get the size of the database after vacuuming
	pagesize = execute_scalar_int(instance, L"pragma page_size");
	pagecount = execute_scalar_int64(instance, L"pragma page_count");

	return pagecount * pagesize;
}

//---------------------------------------------------------------------------

}

#pragma warning(pop)
