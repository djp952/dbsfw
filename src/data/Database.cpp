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

#include <string>

#include "MonsterCard.h"
#include "PrintId.h"
#include "Restriction.h"
#include "SpellCard.h"
#include "SQLiteException.h"
#include "TrapCard.h"

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
// bind_parameter (local)
//
// Used by execute_non_query to bind a Guid parameter
//
// Arguments:
//
//	statement		- SQL statement instance
//	paramindex		- Index of the parameter to bind; will be incremented
//	value			- Value to bind as the parameter

static void bind_parameter(sqlite3_stmt* statement, int& paramindex, Uuid^ value)
{
	int					result;				// Result from binding operation

	// Convert the Guid into a byte array and pin it
	array<Byte>^ guid = value->ToByteArray();
	pin_ptr<Byte> pinguid = &guid[0];

	// Specify SQLITE_TRANSIENT to have SQLite copy the data
	result = sqlite3_bind_blob(statement, paramindex++, pinguid, guid->Length, SQLITE_TRANSIENT);
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
// row_cards (local)
//
// Converts a row from a query against the cards view into a Card object
//
// Arguments:
//
//	database		- Current Database instance
//	statement		- Current sqlite3_stmt pointer

static Card^ row_cards(Database^ database, sqlite3_stmt* statement)
{
	CLRASSERT(CLRISNOTNULL(database));
	CLRASSERT(statement != nullptr);

	// Card instance to be returned to the caller for this result set row
	Card^ card = nullptr;

	// view: cards
	//
	// { 00-06 } cardid | type | name | passcode | text | releasedate | artworkid
	// { 07-12 } monsterattribute | monsterlevel | monstertype | monsterattack | monsterdefense | monsternormal 
	// { 13-19 } monstereffect | monsterfusion | monsterritual | monstertoon | monsterunion | monsterspirit | monstergemini
	// { 20-25 } spellnormal | spellcontinuous | spellequip | spellfield | spellquickplay | spellritual 
	// { 26-28 } trapnormal | trapcontinuous | trapcounter

	// cardid | type
	CardId^ cardid = gcnew CardId(column_uuid(statement, 0));
	CardType type = static_cast<CardType>(sqlite3_column_int(statement, 1));

	// MonsterCard
	if(type == CardType::Monster) {

		MonsterCard^ monster = gcnew MonsterCard(database, cardid);

		monster->Attribute = static_cast<CardAttribute>(sqlite3_column_int(statement, 7));
		monster->Level = sqlite3_column_int(statement, 8);
		monster->Type = static_cast<MonsterType>(sqlite3_column_int(statement, 9));
		monster->Attack = sqlite3_column_int(statement, 10);
		monster->Defense = sqlite3_column_int(statement, 11);
		monster->Normal = (sqlite3_column_int(statement, 12) != 0);
		monster->Effect = (sqlite3_column_int(statement, 13) != 0);
		monster->Fusion = (sqlite3_column_int(statement, 14) != 0);
		monster->Ritual = (sqlite3_column_int(statement, 15) != 0);
		monster->Toon = (sqlite3_column_int(statement, 16) != 0);
		monster->Union = (sqlite3_column_int(statement, 17) != 0);
		monster->Spirit = (sqlite3_column_int(statement, 18) != 0);
		monster->Gemini = (sqlite3_column_int(statement, 19) != 0);

		card = static_cast<Card^>(monster);
	}

	// SpellCard
	else if(type == CardType::Spell) {

		SpellCard^ spell = gcnew SpellCard(database, cardid);

		spell->Normal = (sqlite3_column_int(statement, 20) != 0);
		spell->Continuous = (sqlite3_column_int(statement, 21) != 0);
		spell->Equip = (sqlite3_column_int(statement, 22) != 0);
		spell->Field = (sqlite3_column_int(statement, 23) != 0);
		spell->QuickPlay = (sqlite3_column_int(statement, 24) != 0);
		spell->Ritual = (sqlite3_column_int(statement, 25) != 0);

		card = static_cast<Card^>(spell);
	}

	// TrapCard
	else if(type == CardType::Trap) {

		TrapCard^ trap = gcnew TrapCard(database, cardid);

		trap->Normal = (sqlite3_column_int(statement, 26) != 0);
		trap->Continuous = (sqlite3_column_int(statement, 27) != 0);
		trap->Counter = (sqlite3_column_int(statement, 28) != 0);

		card = static_cast<Card^>(trap);
	}

	// Verify that the Card instance has been set above
	CLRASSERT(CLRISNOTNULL(card));

	// name | passcode | text
	card->Name = column_string(statement, 2);
	card->Passcode = column_string(statement, 3);
	card->Text = column_string(statement, 4);

	// releasedate
	wchar_t const* releasedateptr = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 5));
	card->ReleaseDate = (releasedateptr == nullptr) ? DateTime::MinValue : DateTime::Parse(gcnew String(releasedateptr));

	// artworkid
	card->ArtworkID = gcnew ArtworkId(column_uuid(statement, 6));

	return card;
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
// Database::EnumerateArtwork
//
// Enumerates Artwork from the database
//
// Arguments:
//
//	callback	- Action<> to invoke for each Artwork instance

void Database::EnumerateArtwork(Action<Artwork^>^ callback)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(callback)) throw gcnew ArgumentNullException("callback");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	auto sql = L"select artworkid, cardid, format, width, height, image from artwork";

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// artworkid | cardid
			Artwork^ artwork = gcnew Artwork(this, gcnew ArtworkId(column_uuid(statement, 0)),
				gcnew CardId(column_uuid(statement, 1)));

			// format
			artwork->Format = column_string(statement, 2);

			// width
			artwork->Width = sqlite3_column_int(statement, 3);

			// height
			artwork->Height = sqlite3_column_int(statement, 4);

			// image
			int length = sqlite3_column_bytes(statement, 5);
			if(length > 0) {

				array<Byte>^ image = gcnew array<Byte>(length);

				void const* blob = sqlite3_column_blob(statement, 5);
				if(blob != nullptr) {

					Marshal::Copy(IntPtr(const_cast<void*>(blob)), image, 0, length);
					artwork->Image = image;
				}
			}

			// Invoke the callback and just eat any exceptions that occur
			try { callback->Invoke(artwork); }
			catch(Exception^) { /* DO NOTHING */ }

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::EnumerateCards
//
// Enumerates Cards from the database
//
// Arguments:
//
//	callback	- Action<> to invoke for each Card instance

void Database::EnumerateCards(Action<Card^>^ callback)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(callback)) throw gcnew ArgumentNullException("callback");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	// cards view
	auto sql = L"select * from cards order by name asc";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			try { callback->Invoke(row_cards(this, statement)); }
			catch(Exception^) { /* DO NOTHING */ }

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::EnumerateCards
//
// Enumerates Cards from the database
//
// Arguments:
//
//	callback	- Action<> to invoke for each Card instance
//	releasedate	- Maximum release date for the card

void Database::EnumerateCards(DateTime releasedate, Action<Card^>^ callback)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(callback)) throw gcnew ArgumentNullException("callback");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	// releasedate
	String^ _releasedate = releasedate.ToString("yyyy-MM-dd");
	pin_ptr<wchar_t const> pinmindate = PtrToStringChars(_releasedate);

	// cards view
	auto sql = L"select * from cards where releasedate <= ?1 order by name asc";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_text16(statement, 1, pinmindate, -1, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// Invoke the callback and just eat any exceptions that occur
			try { callback->Invoke(row_cards(this, statement)); }
			catch(Exception^) { /* DO NOTHING */ }

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::EnumerateCardsWithRulings
//
// Enumerates Cards from the database that have Rulings
//
// Arguments:
//
//	callback	- Action<> to invoke for each Card instance

void Database::EnumerateCardsWithRulings(Action<Card^>^ callback)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(callback)) throw gcnew ArgumentNullException("callback");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	// cards view
	auto sql = L"select * from cards where cardid in (select distinct cardid from ruling) order by name asc";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			try { callback->Invoke(row_cards(this, statement)); }
			catch(Exception^) { /* DO NOTHING */ }

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::EnumeratePrints
//
// Enumerates Prints from the database
//
// Arguments:
//
//	callback	- Action<> to invoke for each Print instance

void Database::EnumeratePrints(Action<Print^>^ callback)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(callback)) throw gcnew ArgumentNullException("callback");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	// printid | cardid | seriesid | artworkid | code | language | number | rarity | limitededition | releasedate
	auto sql = L"select print.printid, print.cardid, print.seriesid, print.artworkid, print.code, print.language, "
		"print.number, printrarity(print.rarity), print.releasedate from print "
		"order by print.releasedate asc";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// printid | cardid | seriesid | artworkid
			Print^ print = gcnew Print(this, gcnew PrintId(column_uuid(statement, 0)),
				gcnew CardId(column_uuid(statement, 1)), gcnew SeriesId(column_uuid(statement, 2)),
				gcnew ArtworkId(column_uuid(statement, 3)));

			// code
			print->Code = column_string(statement, 4);

			// language
			print->Language = column_string(statement, 5);

			// number
			print->Number = column_string(statement, 6);

			// rarity
			print->Rarity = static_cast<PrintRarity>(sqlite3_column_int(statement, 7));

			// limitededition
			print->LimitedEdition = sqlite3_column_int(statement, 8) != 0;

			// releasedate
			wchar_t const* releasedateptr = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 9));
			print->ReleaseDate = (releasedateptr == nullptr) ? DateTime::MinValue : DateTime::Parse(gcnew String(releasedateptr));

			// Invoke the callback and just eat any exceptions that occur
			try { callback->Invoke(print); }
			catch(Exception^) { /* DO NOTHING */ }

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::EnumerateRestrictionLists
//
// Enumerates RestrictionLists from the database
//
// Arguments:
//
//	callback	- Action<> to invoke for each Print instance

void Database::EnumerateRestrictionLists(Action<RestrictionList^>^ callback)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(callback)) throw gcnew ArgumentNullException("callback");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	// restrictionlistid | effective | cardid | restriction
	auto sql = L"select restrictionlist.restrictionlistid, restrictionlist.effective from restrictionlist "
		"order by restrictionlist.effective desc";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// restrictionlistid
			RestrictionList^ restrictionlist = gcnew RestrictionList(this, gcnew RestrictionListId(column_uuid(statement, 0)));

			// effective
			wchar_t const* effectiveptr = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 1));
			restrictionlist->EffectiveDate = (effectiveptr == nullptr) ? DateTime::MaxValue : DateTime::Parse(gcnew String(effectiveptr));

			// Invoke the callback and just eat any exceptions that occur
			try { callback->Invoke(restrictionlist); }
			catch(Exception^) { /* DO NOTHING */ }

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::EnumerateRulings
//
// Enumerates Rulings from the database
//
// Arguments:
//
//	callback	- Action<> to invoke for each Ruling instance

void Database::EnumerateRulings(Action<Ruling^>^ callback)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(callback)) throw gcnew ArgumentNullException("callback");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	// sequence | ruling
	auto sql = L"select ruling.sequence, ruling.ruling from ruling order by ruling.cardid, ruling.sequence asc";

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			Ruling^ ruling = gcnew Ruling();

			// sequence
			ruling->Sequence = sqlite3_column_int(statement, 0);

			// text
			ruling->Text = column_string(statement, 1);

			// Invoke the callback and just eat any exceptions that occur
			try { callback->Invoke(ruling); }
			catch(Exception^) { /* DO NOTHING */ }

			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
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
		// cardid(pk) | name(u) | type | passcode(u) | text
		execute_non_query(instance, L"create table card(cardid blob not null, name text unique not null, type text not null, "
			"passcode text unique not null, text text not null, primary key(cardid), "
			"check(type in ('Monster', 'Spell', 'Trap')))");

		// table: monster
		//
		// cardid(fk) | attribute | level | type | attack | defense | normal | effect | fusion | ritual | toon | union | spirit | gemini 
		execute_non_query(instance, L"create table monster(cardid blob not null, attribute text not null, level integer not null, "
			"type text not null, attack integer not null, defense integer not null, normal integer not null, effect integer not null, "
			"fusion integer not null, ritual integer not null, toon integer not null, [union] integer not null, spirit integer not null, "
			"gemini integer not null, foreign key(cardid) references card(cardid), "
			"check(attribute in ('DARK', 'EARTH', 'FIRE', 'LIGHT', 'WATER', 'WIND')), "
			"check(level between 1 and 12), check(attack between -1 and 5000), check(defense between -1 and 5000), "
			"check(type in ('Aqua', 'Beast', 'Beast-Warrior', 'Dinosaur', 'Dragon', 'Fairy', 'Fiend', 'Fish', 'Insect', 'Machine', "
			"  'Plant', 'Pyro', 'Reptile', 'Rock', 'Sea Serpent', 'Spellcaster', 'Thunder', 'Warrior', 'Winged Beast', 'Zombie')), "
			"check(normal = 0 or (effect + fusion + ritual + toon + [union] + spirit + gemini = 0)), "
			"check(fusion = 0 or (toon + [union] + spirit + gemini = 0)), "
			"check(ritual = 0 or (toon + [union] + spirit + gemini = 0)), "
			"check(toon = 0 or effect = 1), check([union] = 0 or effect = 1), check(spirit = 0 or effect = 1), "
			"check(gemini = 0 or effect = 1))");
		execute_non_query(instance, L"create index monster_attribute on monster(attribute)");
		execute_non_query(instance, L"create index monster_level on monster(level)");
		execute_non_query(instance, L"create index monster_type on monster(type)");
		execute_non_query(instance, L"create index monster_attackdefense on monster(attack, defense)");
		execute_non_query(instance, L"create index monster_flag on monster(normal, effect, fusion, ritual, toon, [union], spirit, gemini)");

		// table: spell
		//
		// cardid(fk) | normal | continuous | equip | field | quickplay | ritual
		execute_non_query(instance, L"create table spell(cardid blob not null, normal integer not null, continuous integer not null, "
			"equip integer not null, field integer not null, quickplay integer not null, ritual integer not null, "
			"foreign key(cardid) references card(cardid), "
			"check(normal + continuous + equip + field + quickplay + ritual = 1))");
		execute_non_query(instance, L"create index spell_flag on spell(normal, continuous, equip, field, quickplay, ritual)");

		// table: trap
		//
		// cardid(fk) | normal | continuous | counter
		execute_non_query(instance, L"create table trap(cardid blob not null, normal integer not null, continuous integer not null, "
			"counter integer not null, foreign key(cardid) references card(cardid), "
			"check(normal + continuous + counter = 1))");
		execute_non_query(instance, L"create index trap_flag on trap(normal, continuous, counter)");

		// table: artwork
		//
		// artworkid(pk) | cardid(fk) | format | height | width | image
		execute_non_query(instance, L"create table artwork(artworkid blob not null, cardid blob not null, format text not null,"
			"height integer not null, width integer not null, image blob not null, primary key(artworkid), "
			"foreign key(cardid) references card(cardid))");

		// table: defaultartwork
		//
		// cardid(pk,fk) | artworkid(pk,fk)
		execute_non_query(instance, L"create table defaultartwork(cardid blob not null, artworkid blob null, "
			"primary key(cardid, artworkid), foreign key(cardid) references card(cardid), foreign key(artworkid) references artwork(artworkid))");

		// table: series
		//
		// seriesid(pk) | code(u) | name(u) | releasedate
		execute_non_query(instance, L"create table series(seriesid blob not null, code text unique not null, name text unique not null, "
			"releasedate text null, primary key(seriesid))");
		execute_non_query(instance, L"create index series_releasedate on series(releasedate)");

		// table: print
		//
		// printid(pk) | cardid(fk) | seriesid(fk) | artworkid(fk) | code | language | number | rarity | releasedate
		execute_non_query(instance, L"create table print(printid blob not null, cardid blob not null, seriesid blob not null, "
			"artworkid blob null, code text not null, language text null, number text not null, rarity text not null, releasedate not null, "
			"primary key(printid), foreign key(cardid) references card(cardid), foreign key(seriesid) references series(seriesid), "
			"foreign key(artworkid) references artwork(artworkid))");
		execute_non_query(instance, L"create unique index print_code on print(code, language, number)");
		execute_non_query(instance, L"create index print_rarity on print(rarity)");
		execute_non_query(instance, L"create index print_releasedate on print(releasedate)");

		// table: restrictionlist
		//
		// restrictionlistid(pk) | effective
		execute_non_query(instance, L"create table restrictionlist(restrictionlistid blob not null, effective text unique not null, "
			"primary key(restrictionlistid))");

		// table: restriction
		//
		// restrictionlistid(pk,fk) | cardid(pk,fk) | restriction
		execute_non_query(instance, L"create table restriction(restrictionlistid blob not null, cardid blob not null, restriction integer not null, "
			"primary key(restrictionlistid, cardid), foreign key(restrictionlistid) references restrictionlist(restrictionlistid), "
			"foreign key(cardid) references card(cardid))");

		// table: ruling
		//
		// cardid(fk) | sequence | ruling
		execute_non_query(instance, L"create table ruling(cardid blob not null, sequence integer not null, ruling text not null, "
			"foreign key(cardid) references card(cardid))");

		execute_non_query(instance, L"pragma user_version = 1");
		dbversion = 1;
	}

	// SCHEMA VERSION 1 -> VERSION 2
	//
	// Corrects an incorrect PRIMARY KEY constraint on the defaultartwork table
	if(dbversion == 1) {

		// table: defaultartwork_v1
		//
		// cardid(pk,fk) | artworkid(pk,fk)
		execute_non_query(instance, L"alter table defaultartwork rename to defaultartwork_v1");

		// table: defaultartwork
		//
		// cardid(pk,fk) | artworkid(fk)
		execute_non_query(instance, L"create table defaultartwork(cardid blob not null, artworkid blob not null, "
			"primary key(cardid), foreign key(cardid) references card(cardid), foreign key(artworkid) references artwork(artworkid))");

		// Remove any NULL artworkid values from the old table, move the data into the new table, and drop the old table
		execute_non_query(instance, L"delete from defaultartwork_v1 where artworkid is null");
		execute_non_query(instance, L"insert into defaultartwork select v1.cardid, v1.artworkid from defaultartwork_v1 as v1");
		execute_non_query(instance, L"drop table defaultartwork_v1");

		execute_non_query(instance, L"pragma user_version = 2");
		dbversion = 2;
	}

	// SCHEMA VERSION 2 -> VERSION 3
	//
	// Adds missing CHECK constraint on print.rarity
	// Adds limitededition column to print table
	// Adds boosterpack column to series table
	if(dbversion == 2) {

		// Disable foreign keys during the update
		execute_non_query(instance, L"pragma foreign_keys=OFF");

		// table: series_v2
		//
		// seriesid(pk) | code(u) | name(u) | releasedate
		execute_non_query(instance, L"alter table series rename to series_v2");
		execute_non_query(instance, L"drop index if exists series_releasedate");

		// table: series
		//
		// seriesid(pk) | code(u) | name(u) | boosterpack | releasedate
		execute_non_query(instance, L"create table series(seriesid blob not null, code text unique not null, name text unique not null, "
			"boosterpack integer not null, releasedate text null, primary key(seriesid))");
		execute_non_query(instance, L"create index series_releasedate on series(releasedate)");

		// Move the data from series_v2 into series
		execute_non_query(instance, L"insert into series select v2.seriesid, v2.code, v2.name, 0, v2.releasedate from series_v2 as v2");
		execute_non_query(instance, L"update series set boosterpack = 1 where code in ('LOB', 'MRD', 'SRL', 'PSV', 'LON', 'LOD', 'PGD', "
			"'MFC', 'DCR', 'IOC', 'AST', 'SOD', 'RDS', 'FET', 'TLM', 'CRV', 'EEN', 'SOI', 'EOJ', 'POTD', 'CDIP', 'STON', 'FOTB', 'TAEV', "
			"'GLAS', 'PTDN', 'LOTD')");
		
		// Drop the series_v2 table
		execute_non_query(instance, L"drop table series_v2");

		// table: print_v2
		//
		// printid(pk) | cardid(fk) | seriesid(fk) | artworkid(fk) | code | language | number | rarity | releasedate
		execute_non_query(instance, L"alter table print rename to print_v2");
		execute_non_query(instance, L"drop index if exists print_code");
		execute_non_query(instance, L"drop index if exists print_rarity");
		execute_non_query(instance, L"drop index if exists print_releasedate");

		// table: print
		//
		// printid(pk) | cardid(fk) | seriesid(fk) | artworkid(fk) | code | language | number | rarity | limitededition | releasedate
		execute_non_query(instance, L"create table print(printid blob not null, cardid blob not null, seriesid blob not null, "
			"artworkid blob null, code text not null, language text null, number text not null, rarity text not null, limitededition integer not null, "
			"releasedate text not null, primary key(printid), foreign key(cardid) references card(cardid), foreign key(seriesid) references series(seriesid), "
			"foreign key(artworkid) references artwork(artworkid) "
			"check(rarity in ('Common', 'Gold Rare', 'Parallel Rare', 'Prismatic Secret Rare', 'Rare', 'Secret Rare', 'Super Rare', 'Ultra Parallel Rare', "
			"'Ultra Rare')))");
		execute_non_query(instance, L"create unique index print_code on print(code, language, number)");
		execute_non_query(instance, L"create index print_rarity on print(rarity)");
		execute_non_query(instance, L"create index print_releasedate on print(releasedate)");

		// Move the data from print_v2 into print
		execute_non_query(instance, L"insert into print select v2.printid, v2.cardid, v2.seriesid, v2.artworkid, v2.code, v2.language, v2.number, "
			"v2.rarity, 0, v2.releasedate from print_v2 as v2");

		// Drop the print_v2 table
		execute_non_query(instance, L"drop table print_v2");

		// Enable foreign keys after the update
		execute_non_query(instance, L"pragma foreign_keys=ON");

		execute_non_query(instance, L"pragma user_version = 3");
		dbversion = 3;
	}

	// SCHEMA VERSION 3 -> VERSION 4
	//
	// Add primary keys to the monstercard, spellcard, and trapcard tables
	// Alter restriction.restriction column into text ('Forbidden', 'Limited', 'Semi-Limited')
	// Add primary key to the ruling table
	// Add indexes on print.cardid and print.seriesid columns
	if(dbversion == 3) {

		// Disable foreign keys during the update
		execute_non_query(instance, L"pragma foreign_keys=OFF");

		// table: monster_v3
		//
		// cardid(fk) | attribute | level | type | attack | defense | normal | effect | fusion | ritual | toon | union | spirit | gemini 
		execute_non_query(instance, L"alter table monster rename to monster_v3");
		execute_non_query(instance, L"drop index if exists monster_attribute");
		execute_non_query(instance, L"drop index if exists monster_level");
		execute_non_query(instance, L"drop index if exists monster_type");
		execute_non_query(instance, L"drop index if exists monster_attackdefense");
		execute_non_query(instance, L"drop index if exists monster_flag");

		// table: monster
		//
		// cardid(pk,fk) | attribute | level | type | attack | defense | normal | effect | fusion | ritual | toon | union | spirit | gemini 
		execute_non_query(instance, L"create table monster(cardid blob not null, attribute text not null, level integer not null, "
			"type text not null, attack integer not null, defense integer not null, normal integer not null, effect integer not null, "
			"fusion integer not null, ritual integer not null, toon integer not null, [union] integer not null, spirit integer not null, "
			"gemini integer not null, primary key(cardid), foreign key(cardid) references card(cardid), "
			"check(attribute in ('DARK', 'EARTH', 'FIRE', 'LIGHT', 'WATER', 'WIND')), "
			"check(level between 1 and 12), check(attack between -1 and 5000), check(defense between -1 and 5000), "
			"check(type in ('Aqua', 'Beast', 'Beast-Warrior', 'Dinosaur', 'Dragon', 'Fairy', 'Fiend', 'Fish', 'Insect', 'Machine', "
			"  'Plant', 'Pyro', 'Reptile', 'Rock', 'Sea Serpent', 'Spellcaster', 'Thunder', 'Warrior', 'Winged Beast', 'Zombie')), "
			"check(normal = 0 or (effect + fusion + ritual + toon + [union] + spirit + gemini = 0)), "
			"check(fusion = 0 or (toon + [union] + spirit + gemini = 0)), "
			"check(ritual = 0 or (toon + [union] + spirit + gemini = 0)), "
			"check(toon = 0 or effect = 1), check([union] = 0 or effect = 1), check(spirit = 0 or effect = 1), "
			"check(gemini = 0 or effect = 1))");
		execute_non_query(instance, L"create index monster_attribute on monster(attribute)");
		execute_non_query(instance, L"create index monster_level on monster(level)");
		execute_non_query(instance, L"create index monster_type on monster(type)");
		execute_non_query(instance, L"create index monster_attackdefense on monster(attack, defense)");
		execute_non_query(instance, L"create index monster_flag on monster(normal, effect, fusion, ritual, toon, [union], spirit, gemini)");

		// Move the data from monster_v3 into monster
		execute_non_query(instance, L"insert into monster select v3.cardid, v3.attribute, v3.level, v3.type, v3.attack, v3.defense, v3.normal, "
			"v3.effect, v3.fusion, v3.ritual, v3.toon, v3.[union], v3.spirit, v3.gemini from monster_v3 as v3");
			
		// Drop the monster_v3 table
		execute_non_query(instance, L"drop table monster_v3");

		// table: spell_v3
		//
		// cardid(fk) | normal | continuous | equip | field | quickplay | ritual
		execute_non_query(instance, L"alter table spell rename to spell_v3");
		execute_non_query(instance, L"drop index if exists spell_flag");

		// table: spell
		//
		// cardid(pk,fk) | normal | continuous | equip | field | quickplay | ritual
		execute_non_query(instance, L"create table spell(cardid blob not null, normal integer not null, continuous integer not null, "
			"equip integer not null, field integer not null, quickplay integer not null, ritual integer not null, "
			"primary key(cardid), foreign key(cardid) references card(cardid), "
			"check(normal + continuous + equip + field + quickplay + ritual = 1))");
		execute_non_query(instance, L"create index spell_flag on spell(normal, continuous, equip, field, quickplay, ritual)");

		// Move the data from spell_v3 into spell
		execute_non_query(instance, L"insert into spell select v3.cardid, v3.normal, v3.continuous, v3.equip, v3.field, v3.quickplay, "
			"v3.ritual from spell_v3 as v3");

		// Drop the spell_v3 table
		execute_non_query(instance, L"drop table spell_v3");

		// table: trap_v3
		//
		// cardid(fk) | normal | continuous | counter
		execute_non_query(instance, L"alter table trap rename to trap_v3");
		execute_non_query(instance, L"drop index if exists trap_flag");

		// table: trap
		//
		// cardid(pk, fk) | normal | continuous | counter
		execute_non_query(instance, L"create table trap(cardid blob not null, normal integer not null, continuous integer not null, "
			"counter integer not null, primary key(cardid), foreign key(cardid) references card(cardid), "
			"check(normal + continuous + counter = 1))");
		execute_non_query(instance, L"create index trap_flag on trap(normal, continuous, counter)");

		// Move the data from trap_v3 into trap
		execute_non_query(instance, L"insert into trap select v3.cardid, v3.normal, v3.continuous, v3.counter from trap_v3 as v3");

		// Drop the trap_v3 table
		execute_non_query(instance, L"drop table trap_v3");

		// table: restriction_v3
		//
		// restrictionlistid(pk,fk) | cardid(pk,fk) | restriction
		execute_non_query(instance, L"alter table restriction rename to restriction_v3");

		// table: restriction
		//
		// restrictionlistid(pk,fk) | cardid(pk,fk) | restriction
		execute_non_query(instance, L"create table restriction(restrictionlistid blob not null, cardid blob not null, restriction text not null, "
			"primary key(restrictionlistid, cardid), foreign key(restrictionlistid) references restrictionlist(restrictionlistid), "
			"foreign key(cardid) references card(cardid), check(restriction in ('Forbidden', 'Limited', 'Semi-Limited')))");

		// Move the data from restriction_v3 into restriction
		execute_non_query(instance, L"insert into restriction select v3.restrictionlistid, v3.cardid, "
			"case v3.restriction when 0 then 'Forbidden' when 1 then 'Limited' when 2 then 'Semi-Limited' else 'Unknown' end as restriction "
			"from restriction_v3 as v3");

		// Drop the restriction_v3 table
		execute_non_query(instance, L"drop table restriction_v3");

		// table: ruling_v3
		//
		// cardid(fk) | sequence | ruling
		execute_non_query(instance, L"alter table ruling rename to ruling_v3");

		// table: ruling
		//
		// cardid(pk,fk) | sequence(pk) | ruling
		execute_non_query(instance, L"create table ruling(cardid blob not null, sequence integer not null, ruling text not null, "
			"primary key(cardid, sequence), foreign key(cardid) references card(cardid))");

		// Move the data from ruling_v3 into ruling
		execute_non_query(instance, L"insert into ruling select v3.cardid, v3.sequence, v3.ruling from ruling_v3 as v3");

		// Drop the ruling_v3 table
		execute_non_query(instance, L"drop table ruling_v3");

		// index: print_cardid
		execute_non_query(instance, L"create index print_cardid on print(cardid)");

		// index: print_seriesid
		execute_non_query(instance, L"create index print_seriesid on print(seriesid)");

		// Enable foreign keys after the update
		execute_non_query(instance, L"pragma foreign_keys=ON");

		execute_non_query(instance, L"pragma user_version = 4");
		dbversion = 4;
	}

	// SCHEMA VERSION 4 -> VERSION 5
	//
	// Drop unused indexes
	// Add index on artwork.cardid column
	// Add unique index on series.code column
	if(dbversion == 4) {

		// Drop unused indexes
		execute_non_query(instance, L"drop index if exists monster_attribute");
		execute_non_query(instance, L"drop index if exists monster_level");
		execute_non_query(instance, L"drop index if exists monster_type");
		execute_non_query(instance, L"drop index if exists monster_attackdefense");
		execute_non_query(instance, L"drop index if exists monster_flag");
		execute_non_query(instance, L"drop index if exists spell_flag");
		execute_non_query(instance, L"drop index if exists trap_flag");
		execute_non_query(instance, L"drop index if exists print_rarity");

		// index: artwork.cardid
		execute_non_query(instance, L"create index artwork_cardid on artwork(cardid)");

		// unique index: series.code
		execute_non_query(instance, L"create unique index series_code on series(code)");

		execute_non_query(instance, L"pragma user_version = 5");
		execute_non_query(instance, L"vacuum");
		dbversion = 5;
	}

	CLRASSERT(dbversion == 5);

	// view: cards
	//
	// Denormalizes the card, monstercard, spellcard and trapcard tables into a flat view
	// and also provides the minimum release date for each card for filtering
	//
	// { 00-06 } cardid | type | name | passcode | text | releasedate | artworkid
	// { 07-12 } monsterattribute | monsterlevel | monstertype | monsterattack | monsterdefense | monsternormal 
	// { 13-19 } monstereffect | monsterfusion | monsterritual | monstertoon | monsterunion | monsterspirit | monstergemini
	// { 20-25 } spellnormal | spellcontinuous | spellequip | spellfield | spellquickplay | spellritual 
	// { 26-28 } trapnormal | trapcontinuous | trapcounter
	execute_non_query(instance, L"create temp view cards(cardid, type, name, passcode, text, releasedate, "
		"artworkid, monsterattribute, monsterlevel, monstertype, monsterattack, monsterdefense, monsternormal, "
		"monstereffect, monsterfusion, monsterritual, monstertoon, monsterunion, monsterspirit, monstergemini, "
		"spellnormal, spellcontinuous, spellequip, spellfield, spellquickplay, spellritual, "
		"trapnormal, trapcontinuous, trapcounter) as "
		"with minrelease(cardid, releasedate) as (select cardid, min(releasedate) from print group by cardid) "
		"select card.cardid, cardtype(card.type), card.name, card.passcode, card.text, "
		"(select releasedate from minrelease where cardid = card.cardid), defaultartwork.artworkid, "
		"cardattribute(monster.attribute), monster.level, monstertype(monster.type), monster.attack, "
		"monster.defense, monster.normal, monster.effect, monster.fusion, monster.ritual, "
		"monster.toon, monster.[union], monster.spirit, monster.gemini, "
		"spell.normal, spell.continuous, spell.equip, spell.field, spell.quickplay, spell.ritual, "
		"trap.normal, trap.continuous, trap.counter from card "
		"left outer join defaultartwork on card.cardid = defaultartwork.cardid "
		"left outer join monster on card.cardid = monster.cardid "
		"left outer join spell on card.cardid = spell.cardid "
		"left outer join trap on card.cardid = trap.cardid");
}

//---------------------------------------------------------------------------
// Database::InsertArtwork (internal)
//
// Inserts a new artwork image into the database
//
// Arguments:
//
//	cardid		- CardID for the artwork
//	format		- Image format
//	width		- Image width
//	height		- Image height
//	image		- Image data

ArtworkId^ Database::InsertArtwork(CardId^ cardid, String^ format, int width, int height, array<Byte>^ image)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(cardid)) throw gcnew ArgumentNullException("format");
	if(CLRISNULL(format)) throw gcnew ArgumentNullException("format");
	if(CLRISNULL(image)) throw gcnew ArgumentNullException("image");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	auto sql = L"insert into artwork values(?1, ?2, ?3, ?4, ?5, ?6)";

	// Create a new ArtworkId and pin it
	ArtworkId^ artworkid = gcnew ArtworkId(Guid::NewGuid());
	array<Byte>^ _artworkid = artworkid->ToByteArray();
	pin_ptr<Byte> pinartworkid = &_artworkid[0];

	// Convert the cardid into a byte array and pin it
	array<Byte>^ _cardid = cardid->ToByteArray();
	pin_ptr<Byte> pincardid = &_cardid[0];

	// Pin the format string
	pin_ptr<wchar_t const> pinformat = PtrToStringChars(format);

	// Pin the image data
	pin_ptr<Byte> pinimage = &image[0];

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_blob(statement, 1, pinartworkid, _artworkid->Length, SQLITE_STATIC);
		if(result == SQLITE_OK) result = sqlite3_bind_blob(statement, 2, pincardid, _cardid->Length, SQLITE_STATIC);
		if(result == SQLITE_OK) result = sqlite3_bind_text16(statement, 3, pinformat, -1, SQLITE_STATIC);
		if(result == SQLITE_OK) result = sqlite3_bind_int(statement, 4, height);
		if(result == SQLITE_OK) result = sqlite3_bind_int(statement, 5, width);
		if(result == SQLITE_OK) result = sqlite3_bind_blob(statement, 6, pinimage, image->Length, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query; no rows are expected to be returned
		result = sqlite3_step(statement);
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }

	return artworkid;
}

//---------------------------------------------------------------------------
// Database::Open (static)
//
// Opens an existing database file
//
// Arguments:
//
//	path		- Path on which to open the database file
//	readonly	- Read-only access flag

Database^ Database::Open(String^ path)
{
	// Default to read-only access
	return Open(path, true);
}

//---------------------------------------------------------------------------
// Database::Open (static)
//
// Opens an existing database file
//
// Arguments:
//
//	path		- Path on which to open the database file
//	readonly	- Read-only access flag

Database^ Database::Open(String^ path, bool readonly)
{
	sqlite3* instance = nullptr;

	if(CLRISNULL(path)) throw gcnew ArgumentNullException("path");

	// Create a marshaling context to convert the String^ into an ANSI C-style string
	msclr::auto_handle<msclr::interop::marshal_context> context(gcnew msclr::interop::marshal_context());

	// Attempt to open the database on the specified path
	int result = sqlite3_open_v2(context->marshal_as<char const*>(Path::GetFullPath(path)), &instance,
		(readonly) ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE, nullptr);
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
// Database::SelectArtwork (internal)
//
// Selects an artwork object from the database
//
// Arguments:
//
//	artworkid	- Artwork identifier

Artwork^ Database::SelectArtwork(ArtworkId^ artworkid)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(artworkid)) throw gcnew ArgumentNullException("artworkid");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	// artworkid | cardid | format | width | height | image
	auto sql = L"select artworkid, cardid, format, width, height, image from artwork where artworkid = ?1";

	// Convert the artworkid into a byte array and pin it
	array<Byte>^ _artworkid = artworkid->ToByteArray();
	pin_ptr<Byte> pinartworkid = &_artworkid[0];

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_blob(statement, 1, pinartworkid, _artworkid->Length, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query; there should be at most one row returned
		if(sqlite3_step(statement) == SQLITE_ROW) {

			// artworkid | cardid
			Artwork^ artwork = gcnew Artwork(this, gcnew ArtworkId(column_uuid(statement, 0)), gcnew CardId(column_uuid(statement, 1)));

			// passcode
			artwork->Format = column_string(statement, 2);

			// width
			artwork->Width = sqlite3_column_int(statement, 3);

			// height
			artwork->Height = sqlite3_column_int(statement, 4);

			// image
			int length = sqlite3_column_bytes(statement, 5);
			if(length > 0) {

				array<Byte>^ image = gcnew array<Byte>(length);

				void const* blob = sqlite3_column_blob(statement, 5);
				if(blob != nullptr) {

					Marshal::Copy(IntPtr(const_cast<void*>(blob)), image, 0, length);
					artwork->Image = image;
				}
			}

			return artwork;
		}

		else return nullptr;
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::SelectArtwork (internal)
//
// Selects artwork objects from the database
//
// Arguments:
//
//	cardid	- Card identifier

List<Artwork^>^ Database::SelectArtwork(CardId^ cardid)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(cardid)) throw gcnew ArgumentNullException("cardid");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	List<Artwork^>^ artworks = gcnew List<Artwork^>();

	auto sql = L"select artworkid, cardid, format, width, height, image from artwork where cardid = ?1";

	// Convert the cardid into a byte array and pin it
	array<Byte>^ _cardid = cardid->ToByteArray();
	pin_ptr<Byte> pincardid = &_cardid[0];

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_blob(statement, 1, pincardid, _cardid->Length, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// artworkid | cardid
			Artwork^ artwork = gcnew Artwork(this, gcnew ArtworkId(column_uuid(statement, 0)), gcnew CardId(column_uuid(statement, 1)));

			// passcode
			artwork->Format = column_string(statement, 2);

			// width
			artwork->Width = sqlite3_column_int(statement, 3);

			// height
			artwork->Height = sqlite3_column_int(statement, 4);

			// image
			int length = sqlite3_column_bytes(statement, 5);
			if(length > 0) {

				array<Byte>^ image = gcnew array<Byte>(length);

				void const* blob = sqlite3_column_blob(statement, 5);
				if(blob != nullptr) {

					Marshal::Copy(IntPtr(const_cast<void*>(blob)), image, 0, length);
					artwork->Image = image;
				}
			}

			artworks->Add(artwork);						// Add the Card instance
			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }

	return artworks;
}

//---------------------------------------------------------------------------
// Database::SelectCard (internal)
//
// Selects a single card object from the database
//
// Arguments:
//
//	cardid		- Card identifier

Card^ Database::SelectCard(CardId^ cardid)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	// cards view
	auto sql = L"select * from cards where cardid = ?1";

	// Convert the cardid into a byte array and pin it
	array<Byte>^ _cardid = cardid->ToByteArray();
	pin_ptr<Byte> pincardid = &_cardid[0];

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_blob(statement, 1, pincardid, _cardid->Length, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query; there should be at most one row returned
		if(sqlite3_step(statement) == SQLITE_ROW) return row_cards(this, statement);
		else return nullptr;
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::SelectCards (internal)
//
// Selects Card objects from the database
//
// Arguments:
//
//	restrictionlistid	- RestrictionList to select the cards for
//	restriction			- Restriction within the RestrictionList to select

List<Card^>^ Database::SelectCards(RestrictionListId^ restrictionlistid, Restriction restriction)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(restrictionlistid)) throw gcnew ArgumentNullException("restrictionlistid");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	List<Card^>^ cards = gcnew List<Card^>();

	// cards view
	auto sql = L"select cards.*, restriction(restriction.restriction) from cards "
		"inner join restriction on cards.cardid = restriction.cardid "
		"where restriction.restrictionlistid = ?1 and restriction = restrictionstr(?2) "
		"order by type, name asc";

	// Convert the restrictionlistid into a byte array and pin it
	array<Byte>^ _restrictionlistid = restrictionlistid->ToByteArray();
	pin_ptr<Byte> pinrestrictionlistid = &_restrictionlistid[0];

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_blob(statement, 1, pinrestrictionlistid, _restrictionlistid->Length, SQLITE_STATIC);
		if(result == SQLITE_OK) sqlite3_bind_int(statement, 2, static_cast<int>(restriction));
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			cards->Add(row_cards(this, statement));		// Add the Card instance
			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }

	return cards;
}

//---------------------------------------------------------------------------
// Database::SelectCards (internal)
//
// Selects Card objects from the database
//
// Arguments:
//
//	restrictionlistid	- RestrictionList to select the cards for

Dictionary<Card^, Restriction>^ Database::SelectCards(RestrictionListId^ restrictionlistid)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(restrictionlistid)) throw gcnew ArgumentNullException("restrictionlistid");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	Dictionary<Card^, Restriction>^ cards = gcnew Dictionary<Card^, Restriction>();

	// cards view
	auto sql = L"select cards.*, restriction(restriction.restriction) from cards "
		"inner join restriction on cards.cardid = restriction.cardid "
		"where restriction.restrictionlistid = ?1"
		"order by restriction(restriction.restriction), type, name asc";

	// Convert the restrictionlistid into a byte array and pin it
	array<Byte>^ _restrictionlistid = restrictionlistid->ToByteArray();
	pin_ptr<Byte> pinrestrictionlistid = &_restrictionlistid[0];

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_blob(statement, 1, pinrestrictionlistid, _restrictionlistid->Length, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// Columns 0-28 are consumed by row_cards
			Card^ card = row_cards(this, statement);

			// restriction
			Restriction restriction = static_cast<Restriction>(sqlite3_column_int(statement, 29));

			cards->Add(card, restriction);				// Add the Card instance
			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }

	return cards;
}

//---------------------------------------------------------------------------
// Database::SelectPrints (internal)
//
// Selects Print objects from the database
//
// Arguments:
//
//	cardid		- Card identifier on which to filter the results

List<Print^>^ Database::SelectPrints(CardId^ cardid)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(cardid)) throw gcnew ArgumentNullException("cardid");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	List<Print^>^ prints = gcnew List<Print^>();

	// printid | cardid | seriesid | artworkid | code | language | number | rarity | limitededition | releasedate
	auto sql = L"select print.printid, print.cardid, print.seriesid, print.artworkid, print.code, print.language, "
		"print.number, printrarity(print.rarity), print.limitededition, print.releasedate from print where print.cardid = ?1 "
		"order by print.releasedate asc";

	// Convert the cardid into a byte array and pin it
	array<Byte>^ _cardid = cardid->ToByteArray();
	pin_ptr<Byte> pincardid = &_cardid[0];

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_blob(statement, 1, pincardid, _cardid->Length, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			// printid | cardid | seriesid | artworkid
			Print^ print = gcnew Print(this, gcnew PrintId(column_uuid(statement, 0)),
				gcnew CardId(column_uuid(statement, 1)), gcnew SeriesId(column_uuid(statement, 2)),
				gcnew ArtworkId(column_uuid(statement, 3)));

			// code
			print->Code = column_string(statement, 4);

			// language
			print->Language = column_string(statement, 5);

			// number
			print->Number = column_string(statement, 6);

			// rarity
			print->Rarity = static_cast<PrintRarity>(sqlite3_column_int(statement, 7));

			// limitededition
			print->LimitedEdition = sqlite3_column_int(statement, 8) != 0;

			// releasedate
			wchar_t const* releasedateptr = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 9));
			print->ReleaseDate = (releasedateptr == nullptr) ? DateTime::MinValue : DateTime::Parse(gcnew String(releasedateptr));

			prints->Add(print);							// Add the Print instance
			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }

	return prints;
}

//---------------------------------------------------------------------------
// Database::SelectRulings (internal)
//
// Selects Ruling objects from the database
//
// Arguments:
//
//	cardid		- Card identifier on which to filter the results

List<Ruling^>^ Database::SelectRulings(CardId^ cardid)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(cardid)) throw gcnew ArgumentNullException("cardid");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	List<Ruling^>^ rulings = gcnew List<Ruling^>();

	// sequence | ruling
	auto sql = L"select ruling.sequence, ruling.ruling from ruling where ruling.cardid = ?1 order by ruling.sequence asc";

	// Convert the cardid into a byte array and pin it
	array<Byte>^ _cardid = cardid->ToByteArray();
	pin_ptr<Byte> pincardid = &_cardid[0];

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_blob(statement, 1, pincardid, _cardid->Length, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query and iterate over all returned rows
		result = sqlite3_step(statement);
		while(result == SQLITE_ROW) {

			Ruling^ ruling = gcnew Ruling();

			// sequence
			ruling->Sequence = sqlite3_column_int(statement, 0);

			// text
			ruling->Text = column_string(statement, 1);

			rulings->Add(ruling);						// Add the Ruling instance
			result = sqlite3_step(statement);			// Move to the next result set row
		}

		// If the final result of the query was not SQLITE_DONE, something bad happened
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }

	return rulings;
}

//---------------------------------------------------------------------------
// Database::SelectSeries (internal)
//
// Selects a single series object from the database
//
// Arguments:
//
//	seriesid		- Series identifier

Series^ Database::SelectSeries(SeriesId^ seriesid)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	// seriesid | code | name | boosterpack | releasedate
	auto sql = L"select series.seriesid, series.code, series.name, series.boosterpack, series.releasedate "
		"from series where seriesid = ?1";

	// Convert the seriesid into a byte array and pin it
	array<Byte>^ _seriesid = seriesid->ToByteArray();
	pin_ptr<Byte> pincardid = &_seriesid[0];

	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_blob(statement, 1, pincardid, _seriesid->Length, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query; there should be at most one row returned
		if(sqlite3_step(statement) == SQLITE_ROW) {

			// seriesid
			Series^ series = gcnew Series(this, gcnew SeriesId(column_uuid(statement, 0)));

			// code
			series->Code = column_string(statement, 1);

			// name
			series->Name = column_string(statement, 2);

			// boosterpack
			series->BoosterPack = sqlite3_column_int(statement, 3) != 0;

			// releasedate
			wchar_t const* releasedateptr = reinterpret_cast<wchar_t const*>(sqlite3_column_text16(statement, 4));
			series->ReleaseDate = (releasedateptr == nullptr) ? Nullable<DateTime>() : DateTime::Parse(gcnew String(releasedateptr));

			return series;
		}

		else return nullptr;
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::UpdateArtwork
//
// Updates an artwork image in the database
//
// Arguments:
//
//	artworkid	- Artwork identifier
//	format		- Image format
//	width		- Image width
//	height		- Image height
//	image		- Image data

void Database::UpdateArtwork(ArtworkId^ artworkid, String^ format, int width, int height, array<Byte>^ image)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(artworkid)) throw gcnew ArgumentNullException("artworkid");
	if(CLRISNULL(format)) throw gcnew ArgumentNullException("format");
	if(CLRISNULL(image)) throw gcnew ArgumentNullException("image");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	auto sql = L"update artwork set format = ?1, width = ?2, height = ?3, image = ?4 where artworkid = ?5";

	// Convert the artworkid into a byte array and pin it
	array<Byte>^ _artworkid = artworkid->ToByteArray();
	pin_ptr<Byte> pinartworkid = &_artworkid[0];

	// Pin the format string
	pin_ptr<wchar_t const> pinformat = PtrToStringChars(format);

	// Pin the image data
	pin_ptr<Byte> pinimage = &image[0];

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_text16(statement, 1, pinformat, -1, SQLITE_STATIC);
		if(result == SQLITE_OK) result = sqlite3_bind_int(statement, 2, width);
		if(result == SQLITE_OK) result = sqlite3_bind_int(statement, 3, height);
		if(result == SQLITE_OK) result = sqlite3_bind_blob(statement, 4, pinimage, image->Length, SQLITE_STATIC);
		if(result == SQLITE_OK) result = sqlite3_bind_blob(statement, 5, pinartworkid, _artworkid->Length, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query; no rows are expected to be returned
		result = sqlite3_step(statement);
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::UpdateCardRulings (internal)
//
// Updates the rulings for a Card in the database
//
// Arguments:
//
//	cardid		- Unique identifier
//	rulings		- Enumerable collection of card ruling strings

void Database::UpdateCardRulings(CardId^ cardid, IEnumerable<String^>^ rulings)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(cardid)) throw gcnew ArgumentNullException("cardid");
	if(CLRISNULL(rulings)) throw gcnew ArgumentNullException("rulings");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	auto sql = L"insert into ruling values(?1, ?2, ?3)";

	// Convert the cardid into a byte array and pin it
	array<Byte>^ _cardid = cardid->ToByteArray();
	pin_ptr<Byte> pincardid = &_cardid[0];

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		execute_non_query(instance, L"begin immediate transaction");
		execute_non_query(instance, L"delete from ruling where cardid = ?1", cardid);

		int sequence = 1;
		for each(String^ ruling in rulings)
		{
			// Pin the ruling text
			pin_ptr<wchar_t const> pinruling = PtrToStringChars(ruling);

			// Bind the query parameter(s)
			result = sqlite3_bind_blob(statement, 1, pincardid, _cardid->Length, SQLITE_STATIC);
			if(result == SQLITE_OK) sqlite3_bind_int(statement, 2, sequence);
			if(result == SQLITE_OK) result = sqlite3_bind_text16(statement, 3, pinruling, -1, SQLITE_STATIC);
			if(result != SQLITE_OK) throw gcnew SQLiteException(result);

			// Execute the query; no rows are expected to be returned
			result = sqlite3_step(statement);
			if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

			sqlite3_clear_bindings(statement);				// Clear statement bindings
			sqlite3_reset(statement);						// Reset statement for next iteration

			sequence++;										// Increment the sequence number
		}

		execute_non_query(instance, L"commit transaction");
	}

	catch(Exception^) { execute_non_query(instance, L"rollback transaction"); throw; }

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::UpdateCardText (internal)
//
// Updates the text for a Card in the database
//
// Arguments:
//
//	cardid		- Unique identifier

void Database::UpdateCardText(CardId^ cardid, String^ text)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(cardid)) throw gcnew ArgumentNullException("cardid");
	if(CLRISNULL(text)) throw gcnew ArgumentNullException("text");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	auto sql = L"update card set text = ?1 where cardid = ?2";

	// Convert the cardid into a byte array and pin it
	array<Byte>^ _cardid = cardid->ToByteArray();
	pin_ptr<Byte> pincardid = &_cardid[0];

	// Pin the text string
	pin_ptr<wchar_t const> pintext = PtrToStringChars(text);

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_text16(statement, 1, pintext, -1, SQLITE_STATIC);
		if(result == SQLITE_OK) result = sqlite3_bind_blob(statement, 2, pincardid, _cardid->Length, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query; no rows are expected to be returned
		result = sqlite3_step(statement);
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
}

//---------------------------------------------------------------------------
// Database::UpdateDefaultArtwork
//
// Updates the default artwork for a card in the database
//
// Arguments:
//
//	cardid		- Card unique identifier
//	artworkid	- Artwork unique identifier

void Database::UpdateDefaultArtwork(CardId^ cardid, ArtworkId^ artworkid)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_handle) && (m_handle->IsClosed == false));

	if(CLRISNULL(cardid)) throw gcnew ArgumentNullException("cardid");
	if(CLRISNULL(artworkid)) throw gcnew ArgumentNullException("artworkid");

	SQLiteSafeHandle::Reference instance(m_handle);
	sqlite3_stmt* statement;

	auto sql = L"insert into defaultartwork values(?1, ?2) "
		"on conflict(cardid) do update set artworkid = excluded.artworkid";

	// Convert the cardid into a byte array and pin it
	array<Byte>^ _cardid = cardid->ToByteArray();
	pin_ptr<Byte> pincardid = &_cardid[0];

	// Convert the artworkid into a byte array and pin it
	array<Byte>^ _artworkid = artworkid->ToByteArray();
	pin_ptr<Byte> pinartworkid = &_artworkid[0];

	// Prepare the query
	int result = sqlite3_prepare16_v2(instance, sql, -1, &statement, nullptr);
	if(result != SQLITE_OK) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));

	try {

		// Bind the query parameter(s)
		result = sqlite3_bind_blob(statement, 1, pincardid, _cardid->Length, SQLITE_STATIC);
		if(result == SQLITE_OK) result = sqlite3_bind_blob(statement, 2, pinartworkid, _artworkid->Length, SQLITE_STATIC);
		if(result != SQLITE_OK) throw gcnew SQLiteException(result);

		// Execute the query; no rows are expected to be returned
		result = sqlite3_step(statement);
		if(result != SQLITE_DONE) throw gcnew SQLiteException(result, sqlite3_errmsg(instance));
	}

	finally { sqlite3_finalize(statement); }
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

} // zuki::dbsfw::data

#pragma warning(pop)
