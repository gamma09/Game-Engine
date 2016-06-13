#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <atomic>

class Mutex
{
public:
	class Lock
	{
	public:
		Lock( Mutex& mutex, const std::memory_order lockMemOrder = std::memory_order_acquire );
		~Lock();

	private:
		Mutex& myMutex;

	private:
		Lock() = delete;
		Lock( const Lock& ) = delete;
		Lock& operator=( const Lock& ) = delete;
	};

public:
	Mutex();
	~Mutex();

	void LockMutex( const std::memory_order lockMemoryOrder );
	void UnlockMutex();

private:
	std::atomic_flag functionLock;
	DWORD lockingThreadID;
	unsigned int lockCount;


private:
	Mutex( const Mutex& ) = delete;
	Mutex& operator=( const Mutex& ) = delete;
};