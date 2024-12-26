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
#include "RestrictionList.h"

#include "Card.h"
#include "Database.h"

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// RestrictionList Constructor (internal)
//
// Arguments:
//
//	database			- Underlying Database instance
//	restrictionlistid	- Unique identifier

RestrictionList::RestrictionList(Database^ database, RestrictionListId^ restrictionlistid)
	: m_database(database), m_restrictionlistid(restrictionlistid)
{
	if(CLRISNULL(database)) throw gcnew ArgumentNullException("database");
	if(CLRISNULL(restrictionlistid)) throw gcnew ArgumentNullException("restrictionlistid");
}

//---------------------------------------------------------------------------
// RestrictionList::operator == (static)

bool RestrictionList::operator==(RestrictionList^ lhs, RestrictionList^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return true;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return false;

	return lhs->m_restrictionlistid == rhs->m_restrictionlistid;
}

//---------------------------------------------------------------------------
// RestrictionList::operator != (static)

bool RestrictionList::operator!=(RestrictionList^ lhs, RestrictionList^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return false;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return true;

	return lhs->m_restrictionlistid != rhs->m_restrictionlistid;
}

//---------------------------------------------------------------------------
// RestrictionList::EffectiveDate::get
//
// Gets the RestrictionList effective date

DateTime RestrictionList::EffectiveDate::get(void)
{
	return m_effectivedate;
}

//---------------------------------------------------------------------------
// RestrictionList::EffectiveDate::set (internal)
//
// Sets the RestrictionList effective date

void RestrictionList::EffectiveDate::set(DateTime value)
{
	m_effectivedate = value;
}

//---------------------------------------------------------------------------
// RestrictionList::Equals
//
// Compares this RestrictionList instance to another RestrictionList instance
//
// Arguments:
//
//	rhs		- Right-hand RestrictionList instance to compare against

bool RestrictionList::Equals(RestrictionList^ rhs)
{
	return (this == rhs);
}

//---------------------------------------------------------------------------
// RestrictionList::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool RestrictionList::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a RestrictionList instance
	RestrictionList^ rhsref = dynamic_cast<RestrictionList^>(rhs);
	if(rhsref == nullptr) return false;

	return (this == rhsref);
}

//---------------------------------------------------------------------------
// RestrictionList::GetCards
//
// Gets the Card objects associated with this RestrictionList
//
// Arguments:
//
//	restriction		- Restriction on which to filter the cards

List<Card^>^ RestrictionList::GetCards(Restriction restriction)
{
	CLRASSERT(CLRISNOTNULL(m_database));
	return m_database->SelectCards(m_restrictionlistid, restriction);
}

//---------------------------------------------------------------------------
// RestrictionList::GetCards
//
// Gets the Card objects associated with this RestrictionList
//
// Arguments:
//
//	NONE

Dictionary<Card^, Restriction>^ RestrictionList::GetCards(void)
{
	CLRASSERT(CLRISNOTNULL(m_database));
	return m_database->SelectCards(m_restrictionlistid);
}

//---------------------------------------------------------------------------
// RestrictionList::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int RestrictionList::GetHashCode(void)
{
	return m_restrictionlistid->GetHashCode();
}

//---------------------------------------------------------------------------
// RestrictionList::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ RestrictionList::ToString(void)
{
	return m_effectivedate.ToString("yyyy-MM-dd");
}

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)
