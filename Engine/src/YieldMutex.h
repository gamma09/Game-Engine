#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <atomic>

class YieldMutex
{
public:

	class Lock
	{
	public:
		Lock( YieldMutex& mutex );
		Lock( const Lock& lock );
		Lock& operator=( const Lock& lock );
		~Lock();

	private:
		static void _Lock_Mutex( YieldMutex& mutex );
		static void _Unlock_Mutex( YieldMutex& mutex );


		YieldMutex* mutex;
	};


public:
	YieldMutex();
	~YieldMutex();



private:
	YieldMutex( const YieldMutex& ) = delete;
	YieldMutex( YieldMutex&& ) = delete;
	YieldMutex& operator=( const YieldMutex& ) = delete;
	YieldMutex& operator=( YieldMutex&& ) = delete;

private:
	std::atomic_flag isLocked;
	DWORD owningThreadID;
	unsigned int lockCount;
};
