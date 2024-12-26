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
#include "SpellCard.h"

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// SpellCard Constructor (internal)
//
// Arguments:
//
//	database	- Underlying Database instance
//	cardid		- Unique identifier

SpellCard::SpellCard(Database^ database, CardId^ cardid) : Card(database, cardid, CardType::Spell)
{
}

//---------------------------------------------------------------------------
// SpellCard::Continuous::get
//
// Gets the continuous spell flag

bool SpellCard::Continuous::get(void)
{
	return m_continuous;
}

//---------------------------------------------------------------------------
// SpellCard::Continuous::set (internal)
//
// Sets the continuous spell flag

void SpellCard::Continuous::set(bool value)
{
	m_continuous = value;
}

//---------------------------------------------------------------------------
// SpellCard::Equip::get
//
// Gets the equip spell flag

bool SpellCard::Equip::get(void)
{
	return m_equip;
}

//---------------------------------------------------------------------------
// SpellCard::Equip::set (internal)
//
// Sets the equip spell flag

void SpellCard::Equip::set(bool value)
{
	m_equip = value;
}

//---------------------------------------------------------------------------
// SpellCard::Field::get
//
// Gets the field spell flag

bool SpellCard::Field::get(void)
{
	return m_field;
}

//---------------------------------------------------------------------------
// SpellCard::Field::set (internal)
//
// Sets the field spell flag

void SpellCard::Field::set(bool value)
{
	m_field = value;
}

//---------------------------------------------------------------------------
// SpellCard::Icon::get
//
// Gets the spell card icon

CardIcon SpellCard::Icon::get(void)
{
	if(m_continuous) return CardIcon::Continuous;
	else if(m_equip) return CardIcon::Equip;
	else if(m_field) return CardIcon::Field;
	else if(m_quickplay) return CardIcon::QuickPlay;
	else if(m_ritual) return CardIcon::Ritual;

	return CardIcon::None;
}

//---------------------------------------------------------------------------
// SpellCard::Normal::get
//
// Gets the normal spell flag

bool SpellCard::Normal::get(void)
{
	return m_normal;
}

//---------------------------------------------------------------------------
// SpellCard::Normal::set (internal)
//
// Sets the normal spell flag

void SpellCard::Normal::set(bool value)
{
	m_normal = value;
}

//---------------------------------------------------------------------------
// SpellCard::QuickPlay::get
//
// Gets the quick-play spell flag

bool SpellCard::QuickPlay::get(void)
{
	return m_quickplay;
}

//---------------------------------------------------------------------------
// SpellCard::QuickPlay::set (internal)
//
// Sets the quick-play spell flag

void SpellCard::QuickPlay::set(bool value)
{
	m_quickplay = value;
}

//---------------------------------------------------------------------------
// SpellCard::Ritual::get
//
// Gets the ritual spell flag

bool SpellCard::Ritual::get(void)
{
	return m_ritual;
}

//---------------------------------------------------------------------------
// SpellCard::Ritual::set (internal)
//
// Sets the ritual spell flag

void SpellCard::Ritual::set(bool value)
{
	m_ritual = value;
}

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)
