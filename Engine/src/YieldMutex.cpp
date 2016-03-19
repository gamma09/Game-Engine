#include <thread>
#include <GameAssert.h>

#include "YieldMutex.h"

YieldMutex::Lock::Lock( YieldMutex& inMutex )
	: mutex( &inMutex )
{
	this->_Lock_Mutex( inMutex );
}

YieldMutex::Lock::Lock( const YieldMutex::Lock& lock )
	: mutex( lock.mutex )
{
	GameAssert( mutex->owningThreadID == GetCurrentThreadId() );

	_Lock_Mutex( *this->mutex );
}

YieldMutex::Lock& YieldMutex::Lock::operator=( const YieldMutex::Lock& lock )
{
	GameAssert( lock.mutex->owningThreadID == GetCurrentThreadId() );
	GameAssert( this->mutex->owningThreadID == GetCurrentThreadId() );

	// Lock target mutex
	_Lock_Mutex( *lock.mutex );

	// Change which mutex I point to
	YieldMutex* myOldMutex = this->mutex;
	this->mutex = lock.mutex;
	
	// Then unlock my old mutex
	_Unlock_Mutex( *myOldMutex );

	return *this;
}

YieldMutex::Lock::~Lock()
{
	GameAssert( this->mutex->owningThreadID == GetCurrentThreadId() );

	_Unlock_Mutex( *this->mutex );
}

void YieldMutex::Lock::_Lock_Mutex( YieldMutex& mutex )
{
	while( mutex.isLocked.test_and_set( std::memory_order_acquire ) )
	{
		if( mutex.owningThreadID == GetCurrentThreadId() )
		{
			break;
		}

		std::this_thread::yield();
	}

	mutex.owningThreadID = GetCurrentThreadId();
	mutex.lockCount += 1;
}

void YieldMutex::Lock::_Unlock_Mutex( YieldMutex& mutex )
{
	GameAssert( GetCurrentThreadId() == mutex.owningThreadID );

	mutex.lockCount -= 1;
	if( mutex.lockCount == 0 )
	{
		mutex.owningThreadID = 0;
		mutex.isLocked.clear( std::memory_order_release );
	}
}

YieldMutex::YieldMutex()
	: isLocked(),
	lockCount( 0 ),
	owningThreadID( 0 )
{
	this->isLocked.clear( std::memory_order_release );
}

YieldMutex::~YieldMutex()
{
	// Lock this mutex to ensure no one is using it - whether they try to use it afterward? not our problem
	while( this->isLocked.test_and_set( std::memory_order_acquire ) )
	{
		if( this->owningThreadID == GetCurrentThreadId() )
		{
			break;
		}

		std::this_thread::yield();
	}

	GameAssert( this->lockCount == 0 );
}