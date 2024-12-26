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
#include "Ruling.h"

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Ruling Constructor (internal)
//
// Arguments:
//
//	NONE

Ruling::Ruling()
{
}

//---------------------------------------------------------------------------
// Ruling::operator == (static)

bool Ruling::operator==(Ruling^ lhs, Ruling^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return true;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return false;

	return (lhs->m_sequence == rhs->m_sequence) && (lhs->m_text == rhs->m_text);
}

//---------------------------------------------------------------------------
// Ruling::operator != (static)

bool Ruling::operator!=(Ruling^ lhs, Ruling^ rhs)
{
	if(Object::ReferenceEquals(lhs, rhs)) return false;
	if(Object::ReferenceEquals(lhs, nullptr) || Object::ReferenceEquals(rhs, nullptr)) return true;

	return (lhs->m_sequence != rhs->m_sequence) || (lhs->m_text != rhs->m_text);
}

//---------------------------------------------------------------------------
// Ruling::Sequence::get
//
// Gets the sequence number of the ruling

int Ruling::Sequence::get(void)
{
	return m_sequence;
}

//---------------------------------------------------------------------------
// Ruling::Sequence::set (internal)
//
// Sets the sequence number of the ruling

void Ruling::Sequence::set(int value)
{
	m_sequence = value;
}

//---------------------------------------------------------------------------
// Ruling::Text::get
//
// Gets the ruling text

String^ Ruling::Text::get(void)
{
	return m_text;
}

//---------------------------------------------------------------------------
// Ruling::Code::set (internal)
//
// Sets the ruling text

void Ruling::Text::set(String^ value)
{
	m_text = value;
}

//---------------------------------------------------------------------------
// Ruling::Equals
//
// Compares this Ruling instance to another Ruling instance
//
// Arguments:
//
//	rhs		- Right-hand Ruling instance to compare against

bool Ruling::Equals(Ruling^ rhs)
{
	return (this == rhs);
}

//---------------------------------------------------------------------------
// Ruling::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool Ruling::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a Ruling instance
	Ruling^ rhsref = dynamic_cast<Ruling^>(rhs);
	if(rhsref == nullptr) return false;

	return (this == rhsref);
}

//---------------------------------------------------------------------------
// Ruling::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int Ruling::GetHashCode(void)
{
	return m_sequence.GetHashCode() ^ m_text->GetHashCode();
}

//---------------------------------------------------------------------------
// Ruling::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ Ruling::ToString(void)
{
	return m_text;
}

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)
