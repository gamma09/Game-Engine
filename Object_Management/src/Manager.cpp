#include <GameAssert.h>
#include <string.h>
#include <stdio.h>
#include <ThreadUtils.h>

#include "Manager.h"
#include "ManagedObjectCondition.h"
#include "ManagedObject.h"

Manager* Manager::managersHead = nullptr;
Mutex Manager::managersMutex;
std::thread* Manager::managerThread = nullptr;

Manager::Iterator::Iterator(ManagedObject* head)
	: curr(head)
{
	// Do nothing
}

Manager::Iterator::Iterator(const Iterator& iter) :
	curr(iter.curr)
{
	// DO nothing
}

Manager::Iterator& Manager::Iterator::operator=(const Iterator& iter)
{
	this->curr = iter.curr;
	return *this;
}

Manager::Iterator::~Iterator()
{
	this->curr = 0;
}

ManagedObject* Manager::Iterator::operator++()
{
	ManagedObject* next = this->curr;
	this->curr = this->curr->next;
	return next;
}

ManagedObject* Manager::Iterator::operator++(int)
{
	this->curr = this->curr->next;
	return this->curr;
}

ManagedObject& Manager::Iterator::operator*()
{
	return *this->curr;
}

ManagedObject* Manager::Iterator::operator->() const
{
	return this->curr;
}

const bool Manager::Iterator::Is_At_End() const
{
	return this->curr == 0;
}





Manager::Manager( const uint32_t refill ) :
	refillSize( refill ),
	activeCount( 0 ),
	maxActiveCount( 0 ),
	reserveCount( 0 ),
	activeHead( 0 ),
	reserveHead( 0 ),
	initialSize( 0 ),
	hasCreatedInitialReserve( false ),
	mutex()
{
	// must be able to grow the reserve
	GameAssert(refill > 0);
}

void Manager::Init(uint32_t initial)
{
	this->initialSize = initial;

	Mutex::Lock lock( Manager::managersMutex );
	this->next = Manager::managersHead;
	Manager::managersHead = this;

	if( Manager::managerThread == nullptr )
	{
		Manager::managerThread = new std::thread( Manager::ManagerThreadEntry );
	}
}

Manager::~Manager()
{
}

void Manager::PreDestroy()
{
	this->Remove_Manager();
	this->Destroy_Objects();
}

void Manager::Remove_Manager()
{
	Mutex::Lock lock( Manager::managersMutex );
	Manager* curr = Manager::managersHead;
	Manager* prev = nullptr;
	while( curr && curr != this )
	{
		prev = curr;
		curr = curr->next;
	}

	GameAssert( curr == this );
	if( prev == nullptr )
	{
		Manager::managersHead = curr->next;
	}
	else
	{
		prev->next = curr->next;
	}

	curr->next = nullptr;
}

void Manager::Destroy_Objects()
{
	Mutex::Lock lock( this->mutex );

	while (this->activeCount > 0)
	{
		ManagedObject* curr = this->activeHead;
		this->activeHead = this->activeHead->next;
		curr->Reset();
		this->Delete_Object(curr);
		this->activeCount--;
	}

	while (this->reserveCount > 0)
	{
		ManagedObject* curr = this->reserveHead;
		this->reserveHead = this->reserveHead->next;
		this->Delete_Object(curr);
		this->reserveCount--;
	}
}

void Manager::Print_Statistics(const char* preamble) const
{
	GameAssert(preamble != 0);

	fprintf(stdout, "%s\n", preamble);
	fprintf(stdout, "  Current active count: %d\n", this->activeCount);
	fprintf(stdout, "  Max active count: %d\n", this->maxActiveCount);
	fprintf(stdout, "  Current reserve count: %d\n", this->reserveCount);
}

const Manager::Iterator Manager::Active_Iterator() const
{
	return Iterator(this->activeHead);
}

ManagedObject* Manager::Add_Object()
{
	this->mutex.LockMutex( std::memory_order_acquire );
	while( this->reserveCount.load( std::memory_order_acquire ) == 0 )
	{
		this->mutex.UnlockMutex();
		std::this_thread::yield();
		this->mutex.LockMutex( std::memory_order_acquire );
	}

	ManagedObject* node = this->reserveHead;
	this->reserveHead = this->reserveHead->next;
	node->prev = 0;
	node->next = this->activeHead;

	if (this->activeCount > 0)
		this->activeHead->prev = node;

	this->activeHead = node;

	this->activeCount++;
	this->maxActiveCount = (this->activeCount > this->maxActiveCount) ? this->activeCount : this->maxActiveCount;
	this->reserveCount--;

	this->mutex.UnlockMutex();

	node->isActive = true;

	return node;
}


ManagedObject* Manager::Find_Object(const ManagedObjectCondition& condition) const
{
	for (auto it = this->Active_Iterator(); !it.Is_At_End(); it++)
	{
		if (condition.Matches(*it))
			return &*it;
	}

	return 0;
}

void Manager::Remove( ManagedObject* const obj )
{
	GameAssert( obj );

	Mutex::Lock lock( this->mutex );

	if (obj->prev == 0)
		this->activeHead = obj->next;

	if (this->activeHead != 0)
		this->activeHead->prev = 0;

	obj->Reset();

	obj->next = this->reserveHead;
	this->reserveHead = obj;

	this->activeCount--;
	this->reserveCount++;
}
	
	
	

	
void Manager::Check_Fill_Reserve()
{
	if( !this->hasCreatedInitialReserve )
	{
		Mutex::Lock lock( this->mutex );

		for( uint32_t i = 0; i < this->initialSize; i++ )
		{
			ManagedObject* node = this->Make_Object();
			node->next = this->reserveHead;
			this->reserveHead = node;
		}

		this->reserveCount += this->initialSize;
		this->hasCreatedInitialReserve = true;
	}
	else if( this->reserveCount < this->refillSize )
	{
		Mutex::Lock lock( this->mutex );

		for( uint32_t i = this->reserveCount; i < this->refillSize; i++ )
		{
			ManagedObject* node = this->Make_Object();
			node->next = this->reserveHead;
			this->reserveHead = node;
			this->reserveCount++;
		}
	}
}

void Manager::ManagerThreadEntry()
{
	ThreadUtils::SetThreadName( "Manager Thread" );

	while( Manager::managersHead )
	{
		{
			Mutex::Lock lock( Manager::managersMutex );
			for( Manager* curr = Manager::managersHead; curr != nullptr; curr = curr->next )
			{
				curr->Check_Fill_Reserve();
			}
		}

		std::this_thread::yield();
	}
}