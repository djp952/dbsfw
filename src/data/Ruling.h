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

#ifndef __RULING_H_
#define __RULING_H_
#pragma once

#include "CardID.h"

#pragma warning(push, 4)

using namespace System;

namespace zuki::dbsfw::data {

// FORWARD DECLARATIONS
//
ref class Database;

//---------------------------------------------------------------------------
// Class Ruling
//
// Describes a ruling object
//---------------------------------------------------------------------------

public ref class Ruling
{
public:

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// operator== (static)
	//
	static bool operator==(Ruling^ lhs, Ruling^ rhs);

	// operator!= (static)
	//
	static bool operator!=(Ruling^ lhs, Ruling^ rhs);

	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this Ruling instance to another Ruling instance
	bool Equals(Ruling^ rhs);

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

	// Sequence
	//
	// Gets the sequence number of the ruling
	property int Sequence
	{
		int get(void);
		internal: void set(int value);
	}

	// Text
	//
	// Gets the ruling text
	property String^ Text
	{
		String^ get(void);
		internal: void set(String^ value);
	}

internal:

	// Instance Constructor
	//
	Ruling();

private:

	//-----------------------------------------------------------------------
	// Member Variables

	int						m_sequence = 0;				// Ruling sequence
	String^					m_text = String::Empty;		// Ruling text
};

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)

#endif	// __RULING_H_
