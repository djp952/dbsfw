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
#include "Series.h"

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Series Constructor (internal)
//
// Arguments:
//
//	database	- Underlying Database instance
//	seriesid	- Unique identifier

Series::Series(Database^ database, SeriesId^ seriesid) : m_database(database), m_seriesid(seriesid)
{
	if(CLRISNULL(database)) throw gcnew ArgumentNullException("database");
	if(CLRISNULL(seriesid)) throw gcnew ArgumentNullException("seriesid");
}

//---------------------------------------------------------------------------
// Series::operator == (static)

bool Series::operator==(Series^ lhs, Series^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return true;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return false;

	return lhs->m_seriesid == rhs->m_seriesid;
}

//---------------------------------------------------------------------------
// Series::operator != (static)

bool Series::operator!=(Series^ lhs, Series^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return false;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return true;

	return lhs->m_seriesid != rhs->m_seriesid;
}

//---------------------------------------------------------------------------
// Series::BoosterPack::get
//
// Gets a flag indicating if the series is a booster pack

bool Series::BoosterPack::get(void)
{
	return m_boosterpack;
}

//---------------------------------------------------------------------------
// Series::BoosterPack::set (internal)
//
// Sets a flag indicating if the series is a booster pack

void Series::BoosterPack::set(bool value)
{
	m_boosterpack = value;
}

//---------------------------------------------------------------------------
// Series::Code::get
//
// Gets the series code

String^ Series::Code::get(void)
{
	return m_code;
}

//---------------------------------------------------------------------------
// Series::Code::set (internal)
//
// Sets the series code

void Series::Code::set(String^ value)
{
	m_code = value;
}

//---------------------------------------------------------------------------
// Series::Equals
//
// Compares this Series instance to another Series instance
//
// Arguments:
//
//	rhs		- Right-hand Series instance to compare against

bool Series::Equals(Series^ rhs)
{
	return (this == rhs);
}

//---------------------------------------------------------------------------
// Series::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool Series::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a Series instance
	Series^ rhsref = dynamic_cast<Series^>(rhs);
	if(rhsref == nullptr) return false;

	return (this == rhsref);
}

//---------------------------------------------------------------------------
// Series::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int Series::GetHashCode(void)
{
	return m_seriesid->GetHashCode();
}

//---------------------------------------------------------------------------
// Series::Name::get
//
// Gets the series name

String^ Series::Name::get(void)
{
	return m_name;
}

//---------------------------------------------------------------------------
// Series::Name::set (internal)
//
// Sets the series name

void Series::Name::set(String^ value)
{
	m_name = value;
}

//---------------------------------------------------------------------------
// Series::ReleaseDate::get
//
// Gets the series release date

Nullable<DateTime> Series::ReleaseDate::get(void)
{
	return m_releasedate;
}

//---------------------------------------------------------------------------
// Series::ReleaseDate::set (internal)
//
// Sets the series release date

void Series::ReleaseDate::set(Nullable<DateTime> value)
{
	m_releasedate = value;
}

//---------------------------------------------------------------------------
// Series::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ Series::ToString(void)
{
	return m_name;
}

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)
