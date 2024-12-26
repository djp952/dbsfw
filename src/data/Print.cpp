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
#include "Print.h"

#include "Artwork.h"
#include "Card.h"
#include "Database.h"

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Print Constructor (internal)
//
// Arguments:
//
//	database	- Underlying Database instance
//	printid		- Print unique identifier
//	cardid		- Card unique identifier
//	seriesid	- Series unique identifier
//	artworkid	- Artwork unique identifier

Print::Print(Database^ database, PrintId^ printid, CardId^ cardid, SeriesId^ seriesid, ArtworkId^ artworkid)
	: m_database(database), m_printid(printid), m_cardid(cardid), m_seriesid(seriesid), m_artworkid(artworkid)
{
	if(CLRISNULL(database)) throw gcnew ArgumentNullException("database");
	if(CLRISNULL(printid)) throw gcnew ArgumentNullException("printid");
	if(CLRISNULL(cardid)) throw gcnew ArgumentNullException("cardid");
	if(CLRISNULL(seriesid)) throw gcnew ArgumentNullException("seriesid");
	if(CLRISNULL(artworkid)) throw gcnew ArgumentNullException("artworkid");
}

//---------------------------------------------------------------------------
// Print::operator == (static)

bool Print::operator==(Print^ lhs, Print^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return true;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return false;

	return lhs->m_printid == rhs->m_printid;
}

//---------------------------------------------------------------------------
// Print::operator != (static)

bool Print::operator!=(Print^ lhs, Print^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return false;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return true;

	return lhs->m_printid != rhs->m_printid;
}

//---------------------------------------------------------------------------
// Print::Code::get
//
// Gets the print code

String^ Print::Code::get(void)
{
	return m_code;
}

//---------------------------------------------------------------------------
// Print::Code::set (internal)
//
// Sets the print code

void Print::Code::set(String^ value)
{
	m_code = value;
}

//---------------------------------------------------------------------------
// Print::Equals
//
// Compares this Print instance to another Print instance
//
// Arguments:
//
//	rhs		- Right-hand Print instance to compare against

bool Print::Equals(Print^ rhs)
{
	return (this == rhs);
}

//---------------------------------------------------------------------------
// Print::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool Print::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a Print instance
	Print^ rhsref = dynamic_cast<Print^>(rhs);
	if(rhsref == nullptr) return false;

	return (this == rhsref);
}

//---------------------------------------------------------------------------
// Print::GetArtwork
//
// Gets the artwork associated with the print
//
// Arguments:
//
//	NONE

Artwork^ Print::GetArtwork(void)
{
	CLRASSERT(CLRISNOTNULL(m_database));
	return m_database->SelectArtwork(m_artworkid);
}

//---------------------------------------------------------------------------
// Print::GetCard
//
// Gets the card associated with the print
//
// Arguments:
//
//	NONE

Card^ Print::GetCard(void)
{
	CLRASSERT(CLRISNOTNULL(m_database));
	return m_database->SelectCard(m_cardid);
}

//---------------------------------------------------------------------------
// Print::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int Print::GetHashCode(void)
{
	return m_printid->GetHashCode();
}

//---------------------------------------------------------------------------
// Print::GetSeries
//
// Gets the series associated with the print
//
// Arguments:
//
//	NONE

Series^ Print::GetSeries(void)
{
	CLRASSERT(CLRISNOTNULL(m_database));
	return m_database->SelectSeries(m_seriesid);
}

//---------------------------------------------------------------------------
// Print::Language::get
//
// Gets the print language

String^ Print::Language::get(void)
{
	return m_language;
}

//---------------------------------------------------------------------------
// Print::Language::set (internal)
//
// Sets the print language

void Print::Language::set(String^ value)
{
	m_language = value;
}

//---------------------------------------------------------------------------
// Print::LimitedEdition::get
//
// Gets a flag indicating if the print is a Limited Edition

bool Print::LimitedEdition::get(void)
{
	return m_limitededition;
}

//---------------------------------------------------------------------------
// Print::LimitedEdition::set (internal)
//
// Sets a flag indicating if the print is a Limited Edition

void Print::LimitedEdition::set(bool value)
{
	m_limitededition = value;
}

//---------------------------------------------------------------------------
// Print::Number::get
//
// Gets the print number

String^ Print::Number::get(void)
{
	return m_number;
}

//---------------------------------------------------------------------------
// Print::Number::set (internal)
//
// Sets the print number

void Print::Number::set(String^ value)
{
	m_number = value;
}

//---------------------------------------------------------------------------
// Print::Rarity::get
//
// Gets the print rarity

PrintRarity Print::Rarity::get(void)
{
	return m_rarity;
}

//---------------------------------------------------------------------------
// Print::Rarity::set (internal)
//
// Sets the print rarity

void Print::Rarity::set(PrintRarity value)
{
	m_rarity = value;
}

//---------------------------------------------------------------------------
// Print::ReleaseDate::get
//
// Gets the print release date

DateTime Print::ReleaseDate::get(void)
{
	return m_releasedate;
}

//---------------------------------------------------------------------------
// Print::ReleaseDate::set (internal)
//
// Sets the print release date

void Print::ReleaseDate::set(DateTime value)
{
	m_releasedate = value;
}

//---------------------------------------------------------------------------
// Print::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ Print::ToString(void)
{
	// PSV-000
	// FOTB-EN001
	return String::Concat(m_code, "-", String::IsNullOrEmpty(m_language) ? "" : 
		m_language, m_number);
}

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)
