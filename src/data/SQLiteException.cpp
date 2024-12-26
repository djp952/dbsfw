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

#include "stdafx.h"
#include "SQLiteException.h"

#pragma warning(push, 4)

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// SQLiteException Constructor (internal)
//
// Arguments:
//
//	code		- SQLite result code

SQLiteException::SQLiteException(int code) : m_code(code), Exception(gcnew String(sqlite3_errstr(code)))
{
}

//---------------------------------------------------------------------------
// SQLiteException Constructor (internal)
//
// Arguments:
//
//	code		- SQLite result code
//	message		- SQLite error message

SQLiteException::SQLiteException(int code, String^ message) : m_code(code), 
	Exception(String::IsNullOrEmpty(message) ? gcnew String(sqlite3_errstr(code)) : message)
{
}

//---------------------------------------------------------------------------
// SQLiteException Constructor (internal)
//
// Arguments:
//
//	code		- SQLite result code
//	message		- SQLite error message

SQLiteException::SQLiteException(int code, char const* message) : m_code(code),
	Exception(gcnew String((message == nullptr) ? sqlite3_errstr(code) : message))
{
}

//---------------------------------------------------------------------------
// SQLiteException Constructor (private)
//
// Arguments:
//
//	info		- Serialization information
//	context		- Serialization context

SQLiteException::SQLiteException(SerializationInfo^ info, StreamingContext context) : Exception(info, context)
{
	m_code = info->GetInt32("@m_code");
}

//---------------------------------------------------------------------------
// SQLiteException::Code::get
//
// Gets the SQLite result code

int SQLiteException::Code::get(void)
{
	return m_code;
}

//---------------------------------------------------------------------------
// SQLiteException::GetObjectData
//
// Overrides Exception::GetObjectData
//
// Arguments:
//
//	info		- Serialization information
//	context		- Serialization context

void SQLiteException::GetObjectData(SerializationInfo^ info, StreamingContext context)
{
	if(Object::ReferenceEquals(info, nullptr)) throw gcnew ArgumentNullException("info");
	info->AddValue("@m_code", static_cast<int>(m_code));
	Exception::GetObjectData(info, context);
}

//---------------------------------------------------------------------------

}

#pragma warning(pop)
