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

#ifndef __TRAPCARD_H_
#define __TRAPCARD_H_
#pragma once

#include "Card.h"
#include "CardIcon.h"

#pragma warning(push, 4)

using namespace System;

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Class TrapCard
//
// Describes a trap card object
//---------------------------------------------------------------------------

public ref class TrapCard : public Card
{
public:

	//-----------------------------------------------------------------------
	// Properties

	// Continuous
	//
	// Gets the continuous trap flag
	property bool Continuous
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Counter
	//
	// Gets the continuous trap flag
	property bool Counter
	{
		bool get(void);
		internal: void set(bool value);
	}

	// Icon
	//
	// Gets the trap card icon
	property CardIcon Icon
	{
		CardIcon get(void);
	}

	// Normal
	//
	// Gets the normal trap flag
	property bool Normal
	{
		bool get(void);
		internal: void set(bool value);
	}

internal:

	// Instance Constructor
	//
	TrapCard(Database^ database, CardId^ cardid);

private:

	//-----------------------------------------------------------------------
	// Member Variables

	bool			m_continuous = false;		// Continuous trap
	bool			m_counter = false;			// Counter trap
	bool			m_normal = false;			// Normal trap
};

//---------------------------------------------------------------------------

} // zuki::dbsfw::data

#pragma warning(pop)

#endif	// __TRAPCARD_H_
