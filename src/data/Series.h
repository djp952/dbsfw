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

#ifndef __SERIES_H_
#define __SERIES_H_
#pragma once

#include "SeriesId.h"

#pragma warning(push, 4)

using namespace System;

namespace zuki::dbsfw::data {

// FORWARD DECLARATIONS
//
ref class Database;

//---------------------------------------------------------------------------
// Class Series
//
// Describes a series object
//---------------------------------------------------------------------------

public ref class Series
{
public:

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(Series^ lhs, Series^ rhs);

	// operator!= (static)
	//
	static bool operator!=(Series^ lhs, Series^ rhs);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this Series instance to another Series instance
	bool Equals(Series^ rhs);

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

	// BoosterPack
	//
	// Gets a flag indicating if the series is a booster pack
	property bool BoosterPack
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Code
	//
	// Gets the series code
	property String^ Code
	{
		String^ get(void);
		internal: void set(String^ value);
	}

	// Name
	//
	// Gets the series name
	property String^ Name
	{
		String^ get(void);
		internal: void set(String^ value);
	}

	// ReleaseDate
	//
	// Gets the series release date
	property Nullable<DateTime> ReleaseDate
	{
		Nullable<DateTime> get(void);
		internal: void set(Nullable<DateTime> value);
	}

internal:

	// Instance Constructor
	//
	Series(Database^ database, SeriesId^ seriesid);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	initonly Database^		m_database;					// Database instance
	initonly SeriesId^		m_seriesid;					// Unique identifier

	String^					m_code = String::Empty;		// Series code
	String^					m_name = String::Empty;		// Series name
	bool					m_boosterpack = false;		// Booster pack flag
	Nullable<DateTime>		m_releasedate;				// Series release date
};

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)

#endif	// __SERIES_H_
