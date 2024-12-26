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
#include "Uuid.h"

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Uuid Constructor (protected)
//
// Arguments:
//
//	uuid		- Underlying unique identifier

Uuid::Uuid(Guid uuid) : m_uuid(uuid)
{
}

//---------------------------------------------------------------------------
// Uuid::operator == (static)

bool Uuid::operator==(Uuid^ lhs, Uuid^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return true;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return false;

	return lhs->m_uuid == rhs->m_uuid;
}

//---------------------------------------------------------------------------
// Uuid::operator != (static)

bool Uuid::operator!=(Uuid^ lhs, Uuid^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return false;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return true;

	return lhs->m_uuid != rhs->m_uuid;
}

//---------------------------------------------------------------------------
// Uuid::Equals
//
// Compares this Uuid instance to another Uuid instance
//
// Arguments:
//
//	rhs		- Right-hand Uuid instance to compare against

bool Uuid::Equals(Uuid^ rhs)
{
	return (this == rhs);
}

//---------------------------------------------------------------------------
// Uuid::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool Uuid::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a Uuid instance
	Uuid^ rhsref = dynamic_cast<Uuid^>(rhs);
	if(rhsref == nullptr) return false;

	return (this == rhsref);
}

//---------------------------------------------------------------------------
// Uuid::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int Uuid::GetHashCode(void)
{
	return m_uuid.GetHashCode();
}

//---------------------------------------------------------------------------
// Uuid::ToByteArray
//
// Converts the underlying unique identifier into a byte array
//
// Arguments:
//
//	NONE

array<Byte>^ Uuid::ToByteArray(void)
{
	return m_uuid.ToByteArray();
}

//---------------------------------------------------------------------------
// Uuid::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ Uuid::ToString(void)
{
	return m_uuid.ToString();
}

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)
