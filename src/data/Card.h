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

#ifndef __CARD_H_
#define __CARD_H_
#pragma once

#include "ArtworkId.h"
#include "CardId.h"
#include "CardType.h"

#pragma warning(push, 4)

using namespace System;
using namespace System::Collections::Generic;

namespace zuki::dbsfw::data {

// FORWARD DECLARATIONS
//
ref class Artwork;
ref class Database;
ref class Print;
ref class Ruling;

//---------------------------------------------------------------------------
// Class Card
//
// Describes a card object
//---------------------------------------------------------------------------

public ref class Card abstract
{
public:

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(Card^ lhs, Card^ rhs);

	// operator!= (static)
	//
	static bool operator!=(Card^ lhs, Card^ rhs);

	//-----------------------------------------------------------------------
	// Member Functions

	// AddArtwork
	//
	// Adds new artwork for the card
	void AddArtwork(String^ format, int width, int height, array<Byte>^ image);

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this Card instance to another Card instance
	bool Equals(Card^ rhs);

	// GetArtwork
	//
	// Gets all the artwork associated with the card
	List<Artwork^>^ GetArtwork(void);

	// GetDefaultArtwork
	//
	// Gets the default artwork associated with the card
	Artwork^ GetDefaultArtwork(void);

	// GetHashCode
	//
	// Overrides Object::GetHashCode()
	virtual int GetHashCode(void) override;

	// GetPrints
	//
	// Gets the Print objects associated with this Card
	List<Print^>^ GetPrints(void);

	// GetRulings
	//
	// Gets all the rulings associated with the card
	List<Ruling^>^ GetRulings(void);

	// Refresh
	//
	// Refreshes the information for this Card from the database
	void Refresh(void);

	// ToString
	//
	// Overrides Object::ToString()
	virtual String^ ToString(void) override;

	// UpdateRulings
	//
	// Updates the rulings for this card in the database
	void UpdateRulings(IEnumerable<String^>^ rulings);

	// UpdateText
	//
	// Updates the text for this card in the database
	void UpdateText(String^ text);

	//-----------------------------------------------------------------------
	// Properties

	// Name
	//
	// Gets the card name
	property String^ Name
	{
		String^ get(void);
		internal: void set(String^ value);
	}

	// Passcode
	//
	// Gets the card passcode
	property String^ Passcode
	{
		String^ get(void);
		internal: void set(String^ value);
	}

	// Text
	//
	// Gets the card text
	property String^ Text
	{
		String^ get(void);
		internal: void set(String^ value);
	}

	// Type
	//
	// Gets the card type
	property CardType Type
	{
		CardType get(void);
	}

	// ReleaseDate
	//
	// Gets the card release date
	property DateTime ReleaseDate
	{
		DateTime get(void);
		internal: void set(DateTime value);
	}

internal:

	// Instance Constructor
	//
	Card(Database^ database, CardId^ cardid, CardType type);

	// ArtworkID
	//
	// Sets the artwork unique identifier
	property ArtworkId^ ArtworkID
	{
		internal: void set(ArtworkId^ value);
	}

private:

	//-----------------------------------------------------------------------
	// Member Variables

	initonly CardId^		m_cardid;					// Unique identifier
	initonly CardType		m_type;						// Card type
	initonly Database^		m_database;					// Database instance

	String^					m_name = String::Empty;		// Card name
	String^					m_passcode = String::Empty;	// Card passcode
	String^					m_text = String::Empty;		// Card text
	DateTime				m_releasedate;				// Card release date
	ArtworkId^				m_artworkid;				// Default artwork ID
};

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)

#endif	// __CARD_H_
