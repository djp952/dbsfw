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

#ifndef __DATABASE_H_
#define __DATABASE_H_
#pragma once

#pragma warning(push, 4)

#include "Artwork.h"
#include "ArtworkId.h"
#include "Card.h"
#include "CardId.h"
#include "Print.h"
#include "PrintId.h"
#include "RestrictionList.h"
#include "RestrictionListId.h"
#include "Ruling.h"
#include "SeriesId.h"
#include "SQLiteSafeHandle.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

// dbextension.cpp
//
extern "C" int sqlite3_extension_init(sqlite3* db, char** errmsg, const sqlite3_api_routines* api);

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Class Database
//
// Implements the backing store database operations
//---------------------------------------------------------------------------

public ref class Database
{
public:

	//-----------------------------------------------------------------------
	// Member Functions

	// EnumerateArtwork
	//
	// Enumerates Artwork from the database
	void EnumerateArtwork(Action<Artwork^>^ callback);

	// EnumerateCards
	//
	// Enumerates Cards from the database
	void EnumerateCards(Action<Card^>^ callback);
	void EnumerateCards(DateTime releasedate, Action<Card^>^ callback);

	// EnumerateCardsWithRulings
	//
	// Enumerates Cards from the database that have Rulings
	void EnumerateCardsWithRulings(Action<Card^>^ callback);

	// EnumeratePrints
	//
	// Enumerates Prints from the database
	void EnumeratePrints(Action<Print^>^ callback);

	// EnumerateRestrictionLists
	//
	// Enumerates RestrictionLists from the database
	void EnumerateRestrictionLists(Action<RestrictionList^>^ callback);

	// EnumerateRulings
	//
	// Enumerates Rulings from the database
	void EnumerateRulings(Action<Ruling^>^ callback);

	// Export
	//
	// Exports the database into flat files for storage
	void Export(String^ path);

	// Import
	//
	// Creates a new database instance via import
	static Database^ Import(String^ path, String^ outputfile);

	// Open
	//
	// Opens a new database instance
	static Database^ Open(String^ path);
	static Database^ Open(String^ path, bool readonly);

	// Vacuum
	//
	// Vacuums the database
	int64_t Vacuum(void);
	int64_t Vacuum([OutAttribute] int64_t% oldsize);

internal:

	// InsertArtwork
	//
	// Inserts a new artwork image into the database
	ArtworkId^ InsertArtwork(CardId^ cardid, String^ format, int width, int height, array<Byte>^ image);

	// SelectArtwork
	//
	// Selects a single artwork object from the database
	Artwork^ SelectArtwork(ArtworkId^ artworkid);

	// SelectArtwork
	//
	// Selects artwork objects from the database
	List<Artwork^>^ SelectArtwork(CardId^ cardid);

	// SelectCard
	//
	// Selects a single Card object from the database
	Card^ SelectCard(CardId^ cardid);
	
	// SelectCards
	//
	// Selects Card objects from the database
	List<Card^>^ SelectCards(RestrictionListId^ restrictionlistid, Restriction restriction);
	Dictionary<Card^, Restriction>^ SelectCards(RestrictionListId^ restrictionlistid);

	// SelectPrints
	//
	// Selects Print objects from the database
	List<Print^>^ SelectPrints(CardId^ cardid);

	// SelectRulings
	//
	// Selects Ruling objects from the database
	List<Ruling^>^ SelectRulings(CardId^ cardid);

	// SelectSeries
	//
	// Selects a single Series object from the database
	Series^ SelectSeries(SeriesId^ seriesid);

	// UpdateArtwork
	//
	// Updates an artwork image in the database
	void UpdateArtwork(ArtworkId^ artworkid, String^ format, int width, int height, array<Byte>^ image);

	// UpdateCardRulings
	//
	// Updates the rulings for a Card in the database
	void UpdateCardRulings(CardId^ cardid, IEnumerable<String^>^ rulings);

	// UpdateCardText
	//
	// Updates the text for a Card in the database
	void UpdateCardText(CardId^ cardid, String^ text);

	// UpdateDefaultArtwork
	//
	// Updates the default artwork for a card in the database
	void UpdateDefaultArtwork(CardId^ cardid, ArtworkId^ artworkid);

private:

	// Static Constructor
	//
	static Database();

	// Instance Constructor
	//
	Database(SQLiteSafeHandle^ handle);

	// Destructor
	//
	~Database();

	//-----------------------------------------------------------------------
	// Private Member Functions

	// InitializeInstance (static)
	//
	// Initializes the database instance for use
	static void InitializeInstance(SQLiteSafeHandle^ handle);

	//-----------------------------------------------------------------------
	// Member Variables

	bool					m_disposed = false;		// Object disposal flag
	SQLiteSafeHandle^		m_handle;				// Database safe handle
	
	static int				s_result = SQLITE_OK;	// Result from static init
};

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)

#endif	// __DATABASE_H_
