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

#ifndef __RESTRICTIONLIST_H_
#define __RESTRICTIONLIST_H_
#pragma once

#include "Restriction.h"
#include "RestrictionListId.h"

#pragma warning(push, 4)

using namespace System;
using namespace System::Collections::Generic;

namespace zuki::dbsfw::data {

// FORWARD DECLARATIONS
//
ref class Card;
ref class Database;

//---------------------------------------------------------------------------
// Class RestrictionList
//
// Describes a restriction list object
//---------------------------------------------------------------------------

public ref class RestrictionList
{
public:

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(RestrictionList^ lhs, RestrictionList^ rhs);

	// operator!= (static)
	//
	static bool operator!=(RestrictionList^ lhs, RestrictionList^ rhs);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this RestrictionList instance to another RestrictionList instance
	bool Equals(RestrictionList^ rhs);

	// GetCards
	//
	// Gets the Card objects associated with this RestrictionList
	List<Card^>^ GetCards(Restriction restriction);
	Dictionary<Card^, Restriction>^ GetCards(void);

	// GetHashCode
	//
	// Overrides Object::GetHashCode()
	virtual int GetHashCode(void) override;

	// ToString
	//
	// Overrides Object::ToString()
	virtual String^ ToString(void) override;

	//-----------------------------------------------------------------------
	// Properties

	// EffectiveDate
	//
	// Gets the RestrictionList effective date
	property DateTime EffectiveDate
	{
		DateTime get(void);
		internal: void set(DateTime value);
	}

internal:

	// Instance Constructor
	//
	RestrictionList(Database^ database, RestrictionListId^ restrictionlistid);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	initonly Database^				m_database;				// Database instance
	initonly RestrictionListId^		m_restrictionlistid;	// Unique identifier

	DateTime						m_effectivedate;		// Effective date
};

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)

#endif	// __RESTRICTIONLIST_H_
