#pragma once

#include <stdint.h>

class UpdateEvent
{
public:
	UpdateEvent() : next( nullptr ), prev( nullptr ) {}
	virtual ~UpdateEvent() {}

	// This should return true if the update event should be removed
	virtual bool operator()( uint32_t updateTimeMillis ) abstract;


private:
	UpdateEvent( const UpdateEvent& evt ) = delete;
	UpdateEvent& operator=( const UpdateEvent& evt ) = delete;

private:
	UpdateEvent* next;
	UpdateEvent* prev;



	friend class UpdateEventManager;
};