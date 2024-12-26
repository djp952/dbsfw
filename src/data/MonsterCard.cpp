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
#include "MonsterCard.h"

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// MonsterCard Constructor (internal)
//
// Arguments:
//
//	database	- Underlying Database instance
//	cardid		- Unique indentifier

MonsterCard::MonsterCard(Database^ database, CardId^ cardid) : Card(database, cardid, CardType::Monster)
{
}

//---------------------------------------------------------------------------
// MonsterCard::Attack::get
//
// Gets the monster attack value

int MonsterCard::Attack::get(void)
{
	return m_attack;
}

//---------------------------------------------------------------------------
// MonsterCard::Attack::set (internal)
//
// Sets the monster attack value

void MonsterCard::Attack::set(int value)
{
	m_attack = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Attribute::get
//
// Gets the monster attribute

CardAttribute MonsterCard::Attribute::get(void)
{
	return m_attribute;
}

//---------------------------------------------------------------------------
// MonsterCard::Attribute::set (internal)
//
// Sets the monster attribute

void MonsterCard::Attribute::set(CardAttribute value)
{
	m_attribute = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Defense::get
//
// Gets the monster defense value

int MonsterCard::Defense::get(void)
{
	return m_defense;
}

//---------------------------------------------------------------------------
// MonsterCard::Defense::set (internal)
//
// Sets the monster defense value

void MonsterCard::Defense::set(int value)
{
	m_defense = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Effect::get
//
// Gets the effect monster flag

bool MonsterCard::Effect::get(void)
{
	return m_effect;
}

//---------------------------------------------------------------------------
// MonsterCard::Effect::set (internal)
//
// Sets the effect monster flag

void MonsterCard::Effect::set(bool value)
{
	m_effect = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Fusion::get
//
// Gets the fusion monster flag

bool MonsterCard::Fusion::get(void)
{
	return m_fusion;
}

//---------------------------------------------------------------------------
// MonsterCard::Fusion::set (internal)
//
// Sets the fusion monster flag

void MonsterCard::Fusion::set(bool value)
{
	m_fusion = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Gemini::get
//
// Gets the gemini monster flag

bool MonsterCard::Gemini::get(void)
{
	return m_gemini;
}

//---------------------------------------------------------------------------
// MonsterCard::Gemini::set (internal)
//
// Sets the gemini monster flag

void MonsterCard::Gemini::set(bool value)
{
	m_gemini = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Level::get
//
// Gets the monster level

int MonsterCard::Level::get(void)
{
	return m_level;
}

//---------------------------------------------------------------------------
// MonsterCard::Level::set (internal)
//
// Sets the monster level

void MonsterCard::Level::set(int value)
{
	m_level = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Normal::get
//
// Gets the normal monster flag

bool MonsterCard::Normal::get(void)
{
	return m_normal;
}

//---------------------------------------------------------------------------
// MonsterCard::Normal::set (internal)
//
// Sets the normal monster flag

void MonsterCard::Normal::set(bool value)
{
	m_normal = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Ritual::get
//
// Gets the ritual monster flag

bool MonsterCard::Ritual::get(void)
{
	return m_ritual;
}

//---------------------------------------------------------------------------
// MonsterCard::Ritual::set (internal)
//
// Sets the ritual monster flag

void MonsterCard::Ritual::set(bool value)
{
	m_ritual = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Spirit::get
//
// Gets the spirit monster flag

bool MonsterCard::Spirit::get(void)
{
	return m_spirit;
}

//---------------------------------------------------------------------------
// MonsterCard::Spirit::set (internal)
//
// Sets the spirit monster flag

void MonsterCard::Spirit::set(bool value)
{
	m_spirit = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Toon::get
//
// Gets the toon monster flag

bool MonsterCard::Toon::get(void)
{
	return m_toon;
}

//---------------------------------------------------------------------------
// MonsterCard::Toon::set (internal)
//
// Sets the toon monster flag

void MonsterCard::Toon::set(bool value)
{
	m_toon = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Type::get
//
// Gets the monster type

MonsterType MonsterCard::Type::get(void)
{
	return m_type;
}

//---------------------------------------------------------------------------
// MonsterCard::Type::set (internal)
//
// Sets the monster type

void MonsterCard::Type::set(MonsterType value)
{
	m_type = value;
}

//---------------------------------------------------------------------------
// MonsterCard::Union::get
//
// Gets the union monster flag

bool MonsterCard::Union::get(void)
{
	return m_union;
}

//---------------------------------------------------------------------------
// MonsterCard::Union::set (internal)
//
// Sets the union monster flag

void MonsterCard::Union::set(bool value)
{
	m_union = value;
}

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)
