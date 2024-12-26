//---------------------------------------------------------------------------
// Copyright (c) 2016 Michael G. Brehm
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

#ifndef __SQLITESAFEHANDLE_H_
#define __SQLITESAFEHANDLE_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::Diagnostics;
using namespace System::Runtime::ConstrainedExecution;
using namespace System::Runtime::InteropServices;

//---------------------------------------------------------------------------
// Class SQLiteSafeHandle (internal)
//
// Specialization of SafeHandle for use with a SQLite database handle
//---------------------------------------------------------------------------

ref class SQLiteSafeHandle : public SafeHandle
{
public:

	// Instance Constructor
	//
	SQLiteSafeHandle(sqlite3*&& unmanaged) : SafeHandle(IntPtr::Zero, true)
	{
		// Allocate a new pointer to contain the unmanaged type and take ownership
		try { SetHandle(IntPtr(new sqlite3*(unmanaged))); memset(&unmanaged, 0, sizeof(sqlite3*)); }
		catch(Exception^) { sqlite3_close(unmanaged); throw gcnew OutOfMemoryException(); }
	}

	// Class Reference
	//
	// Accesses the unmanaged type referred to by the safe handle
	ref class Reference
	{
	public:

		// Instance Constructor
		//
		Reference(SQLiteSafeHandle^ handle) : m_handle(handle)
		{
			if(Object::ReferenceEquals(handle, nullptr)) throw gcnew ArgumentNullException("handle");
			
			m_handle->DangerousAddRef(m_release);
			if(!m_release) throw gcnew ObjectDisposedException(SQLiteSafeHandle::typeid->Name);
		}

		// Destructor
		//
		~Reference() { if(m_release) m_handle->DangerousRelease(); }

		// _type conversion operator
		//
		operator sqlite3*() { return *reinterpret_cast<sqlite3**>(m_handle->DangerousGetHandle().ToPointer()); }

	private:

		SQLiteSafeHandle^ m_handle;			// Contained SQLiteSafeHandle instance
		bool m_release = false;				// Flag if handle should be released
	};

	//-----------------------------------------------------------------------
	// Member Functions

	// ReleaseHandle (SafeHandle)
	//
	// Releases the contained unmanaged handle/resource
	[ReliabilityContractAttribute(Consistency::MayCorruptProcess, Cer::Success)]
	virtual bool ReleaseHandle(void) override
	{
		// Cast the handle back into an unmanaged type pointer
		sqlite3** unmanaged = reinterpret_cast<sqlite3**>(handle.ToPointer());

		sqlite3_close(*unmanaged);		// Close the database handle
		delete unmanaged;				// Release the unmanaged heap pointer

#ifdef _DEBUG
		// Dump diagnostic information to the debugger to trace handle lifetimes
		Debug::WriteLine(String::Format("{0}: Disposed [0x{1:X8}]", this->GetType()->Name, handle));
#endif

		return true;
	}

	//-----------------------------------------------------------------------
	// Properties

	// IsInvalid (SafeHandle)
	//
	// Gets a value indicating whether the handle/resource value is invalid
	property bool IsInvalid
	{
		virtual bool get(void) override { return (handle == IntPtr::Zero); }
	}
};

//---------------------------------------------------------------------------

#pragma warning(pop)

#endif	// __SQLITESAFEHANDLE_H_
