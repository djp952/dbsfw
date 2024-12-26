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
#include "Extensions.h"

#pragma warning(push, 4)

using namespace System::ComponentModel;
using namespace System::Reflection;

namespace zuki::dbsfw::data {

//---------------------------------------------------------------------------
// Extensions::EnumDescription (static)
//
// Converts an enum class value into a string based on its [Description] attribute
//
// Arguments:
//
//	value	- Enum class value to be converted

generic<typename T> where T: value class
[ExtensionAttribute]
String^ Extensions::EnumDescription(T value)
{
	if(T::typeid->IsEnum) {

		array<MemberInfo^>^ memberinfo = T::typeid->GetMember(value->ToString());
		if(CLRISNOTNULL(memberinfo) && memberinfo->Length > 0) {

			array<Object^>^ attributes = memberinfo[0]->GetCustomAttributes(DescriptionAttribute::typeid, false);
			if(CLRISNOTNULL(attributes) && attributes->Length > 0) {

				DescriptionAttribute^ description = dynamic_cast<DescriptionAttribute^>(attributes[0]);
				if(CLRISNOTNULL(description)) return description->Description;
			}
		}
	}

	return value->ToString();			// Default to a ToString()
}

//---------------------------------------------------------------------------

}

#pragma warning(pop)
