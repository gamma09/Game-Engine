#pragma once

#ifndef BROWSER_CLI_INTERNAL
#error "BrowserListenerCLI.h may not be included outside of the BrowserCLI package."
#endif

#include <GameAssert.h>
#include "BrowserListener.h"
#include "ContentObjectCLI.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace CLI
{
	public ref class BrowserListenerCLI : CS_Browser::IBrowserListener, Object
	{
	public:
		BrowserListenerCLI( BrowserListener* nativeListener )
		{
			GameAssert( nativeListener != nullptr );

			this->nativeListener = nativeListener;
		}

		virtual void OnModelSelected( CS_Browser::IContentObject^ model )
		{
			ContentObjectCLI^ modelCLI = static_cast<ContentObjectCLI^>( model );

			this->nativeListener->OnModelSelected( modelCLI->GetNativeObject() );
		}

		virtual void OnModelDeselected()
		{
			this->nativeListener->OnModelDeselected();
		}

		virtual void OnActorSelected( CS_Browser::IContentObject^ actor )
		{
			ContentObjectCLI^ actorCLI = static_cast<ContentObjectCLI^>( actor );

			this->nativeListener->OnActorSelected( actorCLI->GetNativeObject() );
		}

		virtual void OnActorDeselected()
		{
			this->nativeListener->OnActorDeselected();
		}

		virtual void OnActorCreated( CS_Browser::IContentObject^ model )
		{
			ContentObjectCLI^ modelCLI = static_cast<ContentObjectCLI^>( model );

			this->nativeListener->OnActorCreated( modelCLI->GetNativeObject() );
		}

		virtual void OnActorDeleted( CS_Browser::IContentObject^ actor )
		{
			ContentObjectCLI^ actorCLI = static_cast<ContentObjectCLI^>( actor );

			this->nativeListener->OnActorDeleted( actorCLI->GetNativeObject() );
		}

		virtual void OnExit()
		{
			this->nativeListener->OnExit();
		}

		virtual void OnNewLevel()
		{
			this->nativeListener->OnNewLevel();
		}

		virtual void OnOpenLevel( String^ filepath )
		{
			char* marshaledFilePath = (char*) Marshal::StringToHGlobalAnsi( filepath ).ToPointer();
			this->nativeListener->OnOpenLevel( marshaledFilePath );
			Marshal::FreeHGlobal( (IntPtr) marshaledFilePath );
		}

		virtual void OnSaveLevel( String^ filepath )
		{
			char* marshaledFilePath = (char*) Marshal::StringToHGlobalAnsi( filepath ).ToPointer();
			this->nativeListener->OnSaveLevel( marshaledFilePath );
			Marshal::FreeHGlobal( (IntPtr) marshaledFilePath );
		}

		virtual void OnLoadModel( String^ filepath )
		{
			char* marshaledFilePath = (char*) Marshal::StringToHGlobalAnsi( filepath ).ToPointer();
			this->nativeListener->OnLoadModel( marshaledFilePath );
			Marshal::FreeHGlobal( (IntPtr) marshaledFilePath );
		}

	private:
		BrowserListener* nativeListener;
	};
}