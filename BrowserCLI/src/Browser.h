#pragma once

#include "BrowserListener.h"

namespace CLI
{
	class BrowserListener;
	class BrowserImpl;
	class ContentObject;


	class Browser : public BrowserListener
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



		virtual void OnModelSelected( ContentObject* model ) override;
		virtual void OnModelDeselected() override;

		virtual void OnActorSelected( ContentObject* actor ) override;
		virtual void OnActorDeselected() override;

		virtual void OnActorCreated( ContentObject* model ) override;
		virtual void OnActorDeleted( ContentObject* actor ) override;

		virtual void OnExit() override;

		virtual void OnNewLevel() override;
		virtual void OnOpenLevel( const char* filepath ) override;
		virtual void OnSaveLevel( const char* filepath ) override;
		virtual void OnLoadModel( const char* filepath ) override;


	private:
		Browser( const Browser& browser ) = delete;
		Browser& operator=( const Browser& browser ) = delete;



	private:
		BrowserImpl* impl;
	};
}