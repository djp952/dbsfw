//---------------------------------------------------------------------------
// Copyright (c) 2025 Michael G. Brehm
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

#ifndef __DATABASE_H_
#define __DATABASE_H_
#pragma once

#pragma warning(push, 4)

#include "SQLiteSafeHandle.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

// dbextension.cpp
//
extern "C" int sqlite3_extension_init(sqlite3* db, char** errmsg, const sqlite3_api_routines* api);

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Class Database
//
// Implements the backing store database operations
//---------------------------------------------------------------------------

public ref class Database
{
public:

	//-----------------------------------------------------------------------
	// Member Functions

	// Export
	//
	// Exports the database into flat files for storage
	void Export(String^ path);

	// Import
	//
	// Creates a new database instance via import
	static Database^ Import(String^ path, String^ outputfile);

	// Open
	//
	// Opens a new database instance
	static Database^ Open(String^ path);

	// Vacuum
	//
	// Vacuums the database
	int64_t Vacuum(void);
	int64_t Vacuum([OutAttribute] int64_t% oldsize);

internal:

private:

	// Static Constructor
	//
	static Database();

	// Instance Constructor
	//
	Database(SQLiteSafeHandle^ handle);

	// Destructor
	//
	~Database();

	//-----------------------------------------------------------------------
	// Private Member Functions

	// InitializeInstance (static)
	//
	// Initializes the database instance for use
	static void InitializeInstance(SQLiteSafeHandle^ handle);

	//-----------------------------------------------------------------------
	// Member Variables

	bool					m_disposed = false;		// Object disposal flag
	SQLiteSafeHandle^		m_handle;				// Database safe handle
	
	static int				s_result = SQLITE_OK;	// Result from static init
};

//---------------------------------------------------------------------------

}

#pragma warning(pop)

#endif	// __DATABASE_H_
