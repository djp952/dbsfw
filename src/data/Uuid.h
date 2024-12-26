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

#ifndef __UUID_H_
#define __UUID_H_
#pragma once

#pragma warning(push, 4)

using namespace System;

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Class Uuid (internal)
//
// Abstraction around a System::Guid
//---------------------------------------------------------------------------

ref class Uuid abstract
{
public:

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(Uuid^ lhs, Uuid^ rhs);

	// operator!= (static)
	//
	static bool operator!=(Uuid^ lhs, Uuid^ rhs);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this Uuid instance to another Uuid instance
	bool Equals(Uuid^ rhs);

	// GetHashCode
	//
	// Overrides Object::GetHashCode()
	virtual int GetHashCode(void) override;

	// ToByteArray
	//
	// Converts the underlying unique identifier into a byte array
	array<Byte>^ ToByteArray(void);

	// ToString
	//
	// Overrides Object::ToString()
	virtual String^ ToString(void) override;

protected:

	// Instance Constructor
	//
	Uuid(Guid uuid);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	Guid			m_uuid;			// Underlying unique identifier
};

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)

#endif	// __UUID_H_
