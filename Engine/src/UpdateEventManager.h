#pragma once

#include <stdint.h>
#include "YieldMutex.h"

// This is not a typical manager type - it doesn't handle allocation of UpdateEvents
// It only handles their destruction

class UpdateEvent;

class UpdateEventManager
{
public:
	UpdateEventManager();
	~UpdateEventManager();

	void AddPreUpdateEvent( UpdateEvent* evt );
	void AddPostUpdateEvent( UpdateEvent* evt );

	void ProcessPreUpdateEvents( uint32_t updateTime );
	void ProcessPostUpdateEvents( uint32_t updateTime );


private:

	UpdateEventManager( const UpdateEventManager& ) = delete;
	UpdateEventManager& operator=( const UpdateEventManager& ) = delete;

private:

	YieldMutex preUpdateMutex;
	UpdateEvent* preUpdateHead;

	YieldMutex postUpdateMutex;
	UpdateEvent* postUpdateHead;
};