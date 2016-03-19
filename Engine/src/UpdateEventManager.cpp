#include <GameAssert.h>

#include "UpdateEventManager.h"
#include "UpdateEvent.h"
#include "MemorySetup.h"

UpdateEventManager::UpdateEventManager()
	: preUpdateHead( nullptr ),
	postUpdateHead( nullptr )
{
	// Do nothing
}

UpdateEventManager::~UpdateEventManager()
{
	{
		YieldMutex::Lock lock( this->preUpdateMutex );
		while( this->preUpdateHead != nullptr )
		{
			UpdateEvent* evt = this->preUpdateHead;
			this->preUpdateHead = this->preUpdateHead->next;
			delete evt;
		}
	}

	{
		YieldMutex::Lock lock( this->postUpdateMutex );
		while( this->postUpdateHead != nullptr )
		{
			UpdateEvent* evt = this->postUpdateHead;
			this->postUpdateHead = this->postUpdateHead->next;
			delete evt;
		}
	}
}

void UpdateEventManager::AddPreUpdateEvent( UpdateEvent* evt )
{
	GameAssert( evt != nullptr );
	GameAssert( evt->next == nullptr );
	GameAssert( evt->prev == nullptr );
	GameAssert( this->preUpdateHead != evt );

	YieldMutex::Lock lock( this->preUpdateMutex );

	evt->next = this->preUpdateHead;
	if( this->preUpdateHead != nullptr )
	{
		this->preUpdateHead->prev = evt;
	}

	this->preUpdateHead = evt;
}

void UpdateEventManager::AddPostUpdateEvent( UpdateEvent* evt )
{
	GameAssert( evt != nullptr );
	GameAssert( evt->next == nullptr );
	GameAssert( evt->prev == nullptr );
	GameAssert( this->postUpdateHead != evt );

	YieldMutex::Lock lock( this->postUpdateMutex );
	evt->next = this->postUpdateHead;
	if( this->postUpdateHead != nullptr )
	{
		this->postUpdateHead->prev = evt;
	}

	this->postUpdateHead = evt->next;
}

void UpdateEventManager::ProcessPreUpdateEvents( uint32_t updateTime )
{
	YieldMutex::Lock lock( this->preUpdateMutex );

	UpdateEvent* curr = this->preUpdateHead;
	while(  curr != nullptr )
	{
		bool shouldDelete = (*curr)( updateTime );
		UpdateEvent* next = curr->next;

		if( shouldDelete )
		{
			if( curr->prev == nullptr )
			{
				this->preUpdateHead = curr->next;
			}
			else
			{
				curr->prev->next = curr->next;
			}

			if( curr->next != nullptr )
			{
				curr->next->prev = curr->prev;
			}

			delete curr;
		}

		curr = next;
	}
}

void UpdateEventManager::ProcessPostUpdateEvents( uint32_t updateTime )
{
	YieldMutex::Lock lock( this->postUpdateMutex );

	UpdateEvent* curr = this->postUpdateHead;
	while( curr != nullptr )
	{
		bool shouldDelete = ( *curr )( updateTime );
		UpdateEvent* next = curr->next;

		if( shouldDelete )
		{
			if( curr->prev == nullptr )
			{
				this->postUpdateHead = curr->next;
			}
			else
			{
				curr->prev->next = curr->next;
			}

			if( curr->next != nullptr )
			{
				curr->next->prev = curr->prev;
			}

			delete curr;
		}

		curr = next;
	}
}