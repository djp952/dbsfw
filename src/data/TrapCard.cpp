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
#include "TrapCard.h"

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// TrapCard Constructor (internal)
//
// Arguments:
//
//	database	- Underlying Database instance
//	cardid		- Unique indentifier

TrapCard::TrapCard(Database^ database, CardId^ cardid) : Card(database, cardid, CardType::Trap)
{
}

//---------------------------------------------------------------------------
// TrapCard::Continuous::get
//
// Gets the continuous trap flag

bool TrapCard::Continuous::get(void)
{
	return m_continuous;
}

//---------------------------------------------------------------------------
// TrapCard::Continuous::set (internal)
//
// Sets the continuous trap flag

void TrapCard::Continuous::set(bool value)
{
	m_continuous = value;
}

//---------------------------------------------------------------------------
// TrapCard::Counter::get
//
// Gets the counter trap flag

bool TrapCard::Counter::get(void)
{
	return m_counter;
}

//---------------------------------------------------------------------------
// TrapCard::Counter::set (internal)
//
// Sets the counter trap flag

void TrapCard::Counter::set(bool value)
{
	m_counter = value;
}

//---------------------------------------------------------------------------
// TrapCard::Icon::get
//
// Gets the spell card icon

CardIcon TrapCard::Icon::get(void)
{
	if(m_continuous) return CardIcon::Continuous;
	else if(m_counter) return CardIcon::Counter;

	return CardIcon::None;
}

//---------------------------------------------------------------------------
// TrapCard::Normal::get
//
// Gets the normal trap flag

bool TrapCard::Normal::get(void)
{
	return m_normal;
}

//---------------------------------------------------------------------------
// TrapCard::Normal::set (internal)
//
// Sets the normal trap flag

void TrapCard::Normal::set(bool value)
{
	m_normal = value;
}

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)
