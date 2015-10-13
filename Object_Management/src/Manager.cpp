#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "Manager.h"
#include "ManagedObjectCondition.h"
#include "ManagedObject.h"


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





Manager::Manager(uint32_t refill) :
	refillSize(refill),
	activeCount(0),
	maxActiveCount(0),
	reserveCount(0),
	activeHead(0),
	reserveHead(0)
{
	// must be able to grow the reserve
	assert(refill > 0);
}

void Manager::Init(uint32_t initial)
{
	this->Fill_Reserve(initial);
}

Manager::~Manager()
{
	// Do nothing
}





void Manager::Destroy_Objects()
{
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
	assert(preamble != 0);

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
	if (this->reserveCount == 0)
		this->Fill_Reserve(this->refillSize);

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

void Manager::Remove(ManagedObject* const obj)
{
	assert(obj != 0);

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
	
	
	

	
void Manager::Fill_Reserve(const uint32_t count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		ManagedObject* node = this->Make_Object();
		node->next = this->reserveHead;
		this->reserveHead = node;
	}

	this->reserveCount += count;
}