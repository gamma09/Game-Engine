#pragma once

namespace CLI
{
	class BrowserListener;
	class BrowserImpl;
	class ContentObject;


	class Browser
	{
	public:
		Browser();
		~Browser();

		void AddBrowserListener( BrowserListener* listener ) const;
		void RemoveBrowserListener( BrowserListener* listener ) const;

		void AddModel( ContentObject* model ) const;
		void RemoveModel( ContentObject* model ) const;

		void AddActor( ContentObject* actor ) const;
		void RemoveActor( ContentObject* actor ) const;

		void Exit() const;
		void SetDirtyFlag( bool bDirty ) const;
		void SetCurrentFile( const char* filename ) const;


	private:
		Browser( const Browser& browser ) = delete;
		Browser& operator=( const Browser& browser ) = delete;



	private:
		BrowserImpl* impl;
	};
}