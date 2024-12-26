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

#ifndef __MONSTERCARD_H_
#define __MONSTERCARD_H_
#pragma once

#include "Card.h"
#include "CardAttribute.h"
#include "MonsterType.h"

#pragma warning(push, 4)

using namespace System;

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Class MonsterCard
//
// Describes a monster card object
//---------------------------------------------------------------------------

public ref class MonsterCard : public Card
{
public:

	//-----------------------------------------------------------------------
	// Properties

	// Attack
	//
	// Gets the monster attack value
	property int Attack
	{
		int get(void);
		internal: void set(int value);
	}

	// Attribute
	//
	// Gets the monster attribute
	property CardAttribute Attribute
	{
		CardAttribute get(void);
		internal: void set(CardAttribute value);
	}

	// Defense
	//
	// Gets the monster defense value
	property int Defense
	{
		int get(void);
		internal: void set(int value);
	}

	// Effect
	//
	// Gets the effect monster flag
	property bool Effect
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Fusion
	//
	// Gets the fusion monster flag
	property bool Fusion
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Gemini
	//
	// Gets the gemini monster flag
	property bool Gemini
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Level
	//
	// Gets the monster level
	property int Level
	{
		int get(void);
		internal: void set(int value);
	}

	// Normal
	//
	// Gets the normal monster flag
	property bool Normal
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Ritual
	//
	// Gets the ritual monster flag
	property bool Ritual
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Spirit
	//
	// Gets the spirit monster flag
	property bool Spirit
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Toon
	//
	// Gets the toon monster flag
	property bool Toon
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Type
	//
	// Gets the monster type
	property MonsterType Type
	{
		MonsterType get(void) new;
		internal: void set(MonsterType value);
	}

	// Union
	//
	// Gets the union monster flag
	property bool Union
	{
		bool get(void);
		internal: void set(bool value);
	}

internal:

	// Instance Constructor
	//
	MonsterCard(Database^ database, CardId^ cardid);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	int					m_attack = 0;			// Attack
	CardAttribute		m_attribute;			// Attribute
	int					m_defense = 0;			// Defense
	bool				m_effect = false;		// Effect monster
	bool				m_fusion = false;		// Fusion monster
	bool				m_gemini = false;		// Gemini monster
	int					m_level = 0;			// Level
	bool				m_normal = false;		// Normal monster
	bool				m_ritual = false;		// Ritual monster
	bool				m_spirit = false;		// Spirit monster
	bool				m_toon = false;			// Toon monster
	bool				m_union = false;		// Union monster
	MonsterType			m_type;					// Type
};

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)

#endif	// __MONSTERCARD_H_
