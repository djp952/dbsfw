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

#ifndef __SPELLCARD_H_
#define __SPELLCARD_H_
#pragma once

#include "Card.h"
#include "CardIcon.h"

#pragma warning(push, 4)

using namespace System;

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Class SpellCard
//
// Describes a spell card object
//---------------------------------------------------------------------------

public ref class SpellCard : public Card
{
public:

	//-----------------------------------------------------------------------
	// Properties

	// Continuous
	//
	// Gets the continuous spell flag
	property bool Continuous
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Equip
	//
	// Gets the equip spell flag
	property bool Equip
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Icon
	//
	// Gets the spell card icon
	property CardIcon Icon
	{
		CardIcon get(void);
	}

	// Field
	//
	// Gets the field spell flag
	property bool Field
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Normal
	//
	// Gets the normal spell flag
	property bool Normal
	{
		bool get(void);
		internal: void set(bool value);
	}

	// QuickPlay
	//
	// Gets the quick-play spell flag
	property bool QuickPlay
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Ritual
	//
	// Gets the ritual spell flag
	property bool Ritual
	{
		bool get(void);
		internal: void set(bool value);
	}

internal:

	// Instance Constructor
	//
	SpellCard(Database^ database, CardId^ cardid);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	bool			m_continuous = false;		// Continuous spell
	bool			m_equip = false;			// Equip spell
	bool			m_field = false;			// Field spell
	bool			m_normal = false;			// Normal spell
	bool			m_quickplay = false;		// Quick-Play spell
	bool			m_ritual = false;			// Ritual spell
};

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)

#endif	// __SPELLCARD_H_
