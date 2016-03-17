#pragma once

#ifndef BROWSER_CLI_INTERNAL
#error "ContentObjectCLI.h may not be included outside of the BrowserCLI package."
#endif

#include <string.h>
#include <GameAssert.h>
#include "ContentObject.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace CLI
{

	public ref class ContentObjectCLI : CS_Browser::IContentObject, Object
	{
	public:
		ContentObjectCLI( ContentObject* nativeObject )
		{
			nativeObject;

			GameAssert( nativeObject != nullptr );
		}

		virtual String^ ToString() override
		{
			const char* name = this->nativeObject->GetName();

			unsigned int size = strlen( name ) + 1;
			char* nameToMarshal = new char[size];
			strcpy_s( nameToMarshal, size, name );
			String^ str = Marshal::PtrToStringAnsi( (IntPtr) nameToMarshal );
			delete nameToMarshal;
			return str;
		}

		inline ContentObject* GetNativeObject() { return this->nativeObject; }

	private:
		ContentObject* nativeObject;
	};
}