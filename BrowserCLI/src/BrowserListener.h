#pragma once

#include "ContentObject.h"


namespace CLI
{
	class BrowserListener
	{
	public:
		BrowserListener() {}
		BrowserListener( const BrowserListener& ) {}
		BrowserListener& operator=( const BrowserListener& ) { return *this; }
		virtual ~BrowserListener() {}

		virtual void OnModelSelected( ContentObject* model ) abstract;
		virtual void OnModelDeselected() abstract;

		virtual void OnActorSelected( ContentObject* actor ) abstract;
		virtual void OnActorDeselected() abstract;

		virtual void OnActorCreated( ContentObject* model ) abstract;
		virtual void OnActorDeleted( ContentObject* actor ) abstract;

		virtual void OnExit() abstract;

		virtual void OnNewLevel() abstract;
		virtual void OnOpenLevel( const char* filepath ) abstract;
		virtual void OnSaveLevel( const char* filepath ) abstract;
		virtual void OnLoadModel( const char* filepath ) abstract;
	};
}