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

#ifndef __PRINT_H_
#define __PRINT_H_
#pragma once

#include "ArtworkId.h"
#include "CardId.h"
#include "PrintId.h"
#include "PrintRarity.h"
#include "Series.h"
#include "SeriesId.h"

#pragma warning(push, 4)

using namespace System;

namespace zuki::dbsfw::data {

// FORWARD DECLARATIONS
//
ref class Artwork;
ref class Card;
ref class Database;

//---------------------------------------------------------------------------
// Class Print
//
// Describes a print object
//---------------------------------------------------------------------------

public ref class Print
{
public:

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(Print^ lhs, Print^ rhs);

	// operator!= (static)
	//
	static bool operator!=(Print^ lhs, Print^ rhs);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this Print instance to another Print instance
	bool Equals(Print^ rhs);

	// GetArtwork
	//
	// Gets the artwork associated with the print
	Artwork^ GetArtwork(void);

	// GetCard
	//
	// Gets the Card associated with the print
	Card^ GetCard(void);

	// GetHashCode
	//
	// Overrides Object::GetHashCode()
	virtual int GetHashCode(void) override;

	// GetSeries
	//
	// Gets the Series associated with the print
	Series^ GetSeries(void);

	// ToString
	//
	// Overrides Object::ToString()
	virtual String^ ToString(void) override;

	//-----------------------------------------------------------------------
	// Properties

	// Code
	//
	// Gets the print code
	property String^ Code
	{
		String^ get(void);
		internal: void set(String^ value);
	}

	// Language
	//
	// Gets the print language
	property String^ Language
	{
		String^ get(void);
		internal: void set(String^ value);
	}

	// LimitedEdition
	//
	// Gets a flag indicating if the print is a Limited Edition
	property bool LimitedEdition
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Number
	//
	// Gets the print number
	property String^ Number
	{
		String^ get(void);
		internal: void set(String^ value);
	}

	// Rarity
	//
	// Gets the print rarity
	property PrintRarity Rarity
	{
		PrintRarity get(void);
		internal: void set(PrintRarity value);
	}

	// ReleaseDate
	//
	// Gets the print release date
	property DateTime ReleaseDate
	{
		DateTime get(void);
		internal: void set(DateTime value);
	}

internal:

	// Instance Constructor
	//
	Print(Database^ database, PrintId^ printid, CardId^ cardid, SeriesId^ seriesid, ArtworkId^ artworkid);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	initonly Database^		m_database;					// Database instance
	initonly PrintId^		m_printid;					// Unique identifier
	initonly CardId^		m_cardid;					// Unique identifer
	initonly SeriesId^		m_seriesid;					// Unique identifier
	initonly ArtworkId^		m_artworkid;				// Unique identifier

	String^					m_code = String::Empty;		// Print code
	String^					m_language = String::Empty;	// Language code
	String^					m_number = String::Empty;	// Print number
	PrintRarity				m_rarity;					// Print rarity
	bool					m_limitededition = false;	// Limited edition flag
	DateTime				m_releasedate;				// Print release date
};

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)

#endif	// __PRINT_H_
