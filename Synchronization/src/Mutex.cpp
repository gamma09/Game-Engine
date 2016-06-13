#include <thread>
#include <GameAssert.h>
#include "Mutex.h"

Mutex::Lock::Lock( Mutex& mutex, const std::memory_order lockMemOrder )
	: myMutex( mutex )
{
	this->myMutex.LockMutex( lockMemOrder );
}

Mutex::Lock::~Lock()
{
	this->myMutex.UnlockMutex();
}

Mutex::Mutex()
	: lockingThreadID( 0 ),
	lockCount( 0 ),
	functionLock()
{
	this->functionLock.clear();
}

Mutex::~Mutex()
{
}

void Mutex::LockMutex( const std::memory_order lockMemoryOrder )
{
	GameAssert( lockMemoryOrder == std::memory_order_acquire || lockMemoryOrder == std::memory_order_seq_cst );

	DWORD currentThreadID = GetCurrentThreadId();
	bool shouldRun = true;

	do
	{
		while( this->functionLock.test_and_set( std::memory_order_acquire ) )
		{
			std::this_thread::yield();
		}

		if( this->lockingThreadID == 0 || this->lockingThreadID == currentThreadID )
		{
			this->lockingThreadID = currentThreadID;
			this->lockCount++;
			shouldRun = false;
		}

		this->functionLock.clear( std::memory_order_release );
		std::this_thread::yield();
	} while( shouldRun );
}

void Mutex::UnlockMutex()
{
	while( this->functionLock.test_and_set( std::memory_order_acquire ) )
	{
		std::this_thread::yield();
	}

	GameAssert( this->lockCount > 0 );

	this->lockCount--;
	if( this->lockCount == 0 )
	{
		this->lockingThreadID = 0;
	}

	this->functionLock.clear( std::memory_order_release );
}