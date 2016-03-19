#include <GameAssert.h>
#include <vcclr.h>

#include "Browser.h"
#include "BrowserListener.h"
#include "ContentObject.h"
#include "CLIObjectTable.h"
#include "BrowserListenerCLI.h"

using namespace CS_Browser;

namespace CLI
{

	class BrowserImpl
	{
	public:
		BrowserImpl()
			: browser( gcnew CS_Browser::Browser() ),
			table( 20 )
		{
			// Do nothing
		}

		~BrowserImpl()
		{
			this->browser = nullptr;
		}



		gcroot<CS_Browser::Browser^> browser;
		CLIObjectTable table;

	private:
		BrowserImpl( const BrowserImpl& impl ) = delete;
		BrowserImpl& operator=( const BrowserImpl& impl ) = delete;
	};





	Browser::Browser()
		: impl( new BrowserImpl() )
	{
		this->AddBrowserListener( this );
	}

	Browser::~Browser()
	{
		if( this->impl != nullptr )
		{
			delete this->impl;
		}
	}

	void Browser::AddBrowserListener( BrowserListener* listener ) const
	{
		GameAssert( listener != nullptr );

		if( this->impl != nullptr )
		{
			BrowserListenerCLI^ listenerCLI = gcnew BrowserListenerCLI( listener );
			this->impl->table.Add( listener, listenerCLI );
			this->impl->browser->AddBrowserListener( listenerCLI );
		}
	}

	void Browser::RemoveBrowserListener( BrowserListener* listener ) const
	{
		GameAssert( listener != nullptr );

		if( this->impl != nullptr )
		{
			BrowserListenerCLI^ listenerCLI = static_cast<BrowserListenerCLI^>( this->impl->table.Remove( listener ) );
			if( listenerCLI != nullptr )
			{
				this->impl->browser->RemoveBrowserListener( listenerCLI );
			}
		}
	}

	void Browser::AddModel( ContentObject* model ) const
	{
		GameAssert( model != nullptr );

		if( this->impl != nullptr )
		{
			ContentObjectCLI^ modelCLI = gcnew ContentObjectCLI( model );
			this->impl->table.Add( model, modelCLI );
			this->impl->browser->AddModel( modelCLI );
		}
	}

	void Browser::RemoveModel( ContentObject* model ) const
	{
		GameAssert( model != nullptr );

		if( this->impl != nullptr )
		{
			ContentObjectCLI^ modelCLI = static_cast<ContentObjectCLI^>( this->impl->table.Remove( model ) );
			if( modelCLI != nullptr )
			{
				this->impl->browser->RemoveModel( modelCLI );
			}
		}
	}

	void Browser::AddActor( ContentObject* actor ) const
	{
		GameAssert( actor != nullptr );

		if( this->impl != nullptr )
		{
			if( this->impl != nullptr )
			{
				ContentObjectCLI^ actorCLI = gcnew ContentObjectCLI( actor );
				this->impl->table.Add( actor, actorCLI );
				this->impl->browser->AddActor( actorCLI );
			}
		}
	}

	void Browser::RemoveActor( ContentObject* actor ) const
	{
		GameAssert( actor != nullptr );

		if( this->impl != nullptr )
		{

			ContentObjectCLI^ actorCLI = static_cast<ContentObjectCLI^>( this->impl->table.Remove( actor ) );
			if( actorCLI != nullptr )
			{
				this->impl->browser->RemoveActor( actorCLI );
			}
		}
	}

	void Browser::Exit() const
	{
		if( this->impl != nullptr )
		{
			this->impl->browser->Exit();
		}
	}

	void Browser::SetDirtyFlag( bool bDirty ) const
	{
		if( this->impl != nullptr )
		{
			this->impl->browser->SetFileDirtyFlag( bDirty );
		}
	}

	void Browser::SetCurrentFile( const char* filename ) const
	{
		if( this->impl != nullptr )
		{
			unsigned int size = strlen( filename ) + 1;
			char* localCopy = new char[size];
			strcpy_s( localCopy, size, filename );

			String^ managedFilename = Marshal::PtrToStringAnsi( (IntPtr) localCopy );
			this->impl->browser->SetCurrentFile( managedFilename );

			delete localCopy;
		}
	}

	void Browser::OnModelSelected( ContentObject* ) {}
	void Browser::OnModelDeselected() {}

	void Browser::OnActorSelected( ContentObject* ) {}
	void Browser::OnActorDeselected() {}

	void Browser::OnActorCreated( ContentObject* ) {}
	void Browser::OnActorDeleted( ContentObject* ) {}

	void Browser::OnExit()
	{
		if( this->impl != nullptr )
		{
			BrowserImpl* impl = this->impl;
			this->impl = nullptr;

			delete impl;
		}
	}

	void Browser::OnNewLevel() {}
	void Browser::OnOpenLevel( const char* ) {}
	void Browser::OnSaveLevel( const char* ) {}
	void Browser::OnLoadModel( const char* ) {}
}