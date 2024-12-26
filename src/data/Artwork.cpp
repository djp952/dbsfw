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
#include "Artwork.h"

#include "Card.h"
#include "Database.h"

#pragma warning(push, 4)

using namespace System::IO;

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Artwork Constructor (internal)
//
// Arguments:
//
//	database	- Underlying Database instance
//	artworkid	- Artwork unique identifier
//	cardid		- Card unique identifier

Artwork::Artwork(Database^ database, ArtworkId^ artworkid, CardId^ cardid) : m_database(database),
	m_artworkid(artworkid), m_cardid(cardid)
{
	if(CLRISNULL(database)) throw gcnew ArgumentNullException("database");
	if(CLRISNULL(artworkid)) throw gcnew ArgumentNullException("artworkid");
	if(CLRISNULL(cardid)) throw gcnew ArgumentNullException("cardid");
}

//---------------------------------------------------------------------------
// Artwork Destructor (private)

Artwork::~Artwork()
{
	if(m_disposed) return;

	if(CLRISNOTNULL(m_image)) delete m_image;

	m_disposed = true;
}

//---------------------------------------------------------------------------
// Artwork::operator == (static)

bool Artwork::operator==(Artwork^ lhs, Artwork^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return true;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return false;

	return lhs->m_artworkid == rhs->m_artworkid;
}

//---------------------------------------------------------------------------
// Artwork::operator != (static)

bool Artwork::operator!=(Artwork^ lhs, Artwork^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return false;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return true;

	return lhs->m_artworkid != rhs->m_artworkid;
}

//---------------------------------------------------------------------------
// Artwork::Equals
//
// Compares this Artwork instance to another Artwork instance
//
// Arguments:
//
//	rhs		- Right-hand Artwork instance to compare against

bool Artwork::Equals(Artwork^ rhs)
{
	CHECK_DISPOSED(m_disposed);
	return (this == rhs);
}

//---------------------------------------------------------------------------
// Artwork::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool Artwork::Equals(Object^ rhs)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a Artwork instance
	Artwork^ rhsref = dynamic_cast<Artwork^>(rhs);
	if(rhsref == nullptr) return false;

	return (this == rhsref);
}

//---------------------------------------------------------------------------
// Artwork::Format::get
//
// Gets the artwork format

String^ Artwork::Format::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_format;
}

//---------------------------------------------------------------------------
// Artwork::Format::set (internal)
//
// Sets the artwork format

void Artwork::Format::set(String^ value)
{
	CHECK_DISPOSED(m_disposed);
	m_format = value;
}

//---------------------------------------------------------------------------
// Artwork::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int Artwork::GetHashCode(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_artworkid->GetHashCode();
}

//---------------------------------------------------------------------------
// Artwork::Height::get
//
// Gets the artwork height

int Artwork::Height::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_height;
}

//---------------------------------------------------------------------------
// Artwork::Height::set (internal)
//
// Sets the artwork height

void Artwork::Height::set(int value)
{
	CHECK_DISPOSED(m_disposed);
	m_height = value;
}

//---------------------------------------------------------------------------
// Artwork::Image::get
//
// Gets the artwork image

array<Byte>^ Artwork::Image::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_image;
}

//---------------------------------------------------------------------------
// Artwork::Image::set (internal)
//
// Sets the artwork image

void Artwork::Image::set(array<Byte>^ value)
{
	CHECK_DISPOSED(m_disposed);
	if(CLRISNOTNULL(m_image)) delete m_image;
	m_image = value;
}

//---------------------------------------------------------------------------
// Artwork::SetDefault
//
// Sets the artwork as the default for the card
//
// Arguments:
//
//	NONE

void Artwork::SetDefault(void)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_database));

	m_database->UpdateDefaultArtwork(m_cardid, m_artworkid);
}

//---------------------------------------------------------------------------
// Artwork::ToBitmap
//
// Converts the raw image data into a Bitmap
//
// Arguments:
//
//	NONE

Bitmap^ Artwork::ToBitmap(void)
{
	CHECK_DISPOSED(m_disposed);
	if(CLRISNULL(m_image)) return nullptr;

	msclr::auto_handle<MemoryStream> stream(gcnew MemoryStream(m_image));
	return gcnew Bitmap(stream.get());
}

//---------------------------------------------------------------------------
// Artwork::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ Artwork::ToString(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_artworkid->ToString();
}

//---------------------------------------------------------------------------
// Artwork::UpdateImage
//
// Updates the artwork image
//
// Arguments:
//
//	format		- Image format
//	width		- Image width
//	height		- Image height
//	image		- Image data

void Artwork::UpdateImage(String^ format, int width, int height, array<Byte>^ image)
{
	CHECK_DISPOSED(m_disposed);
	CLRASSERT(CLRISNOTNULL(m_database));

	if(CLRISNULL(format)) throw gcnew ArgumentNullException("format");
	if(CLRISNULL(image)) throw gcnew ArgumentNullException("image");

	// Attempt to update the image in the database
	m_database->UpdateArtwork(m_artworkid, format, width, height, image);

	m_format = format;
	m_width = width;
	m_height = height;

	if(CLRISNOTNULL(m_image)) delete m_image;
	m_image = image;
}

//---------------------------------------------------------------------------
// Artwork::Width::get
//
// Gets the artwork width

int Artwork::Width::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_width;
}

//---------------------------------------------------------------------------
// Artwork::Width::set (internal)
//
// Sets the artwork width

void Artwork::Width::set(int value)
{
	CHECK_DISPOSED(m_disposed);
	m_width = value;
}

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)
