#pragma once

#include <stdint.h>

class ManagedObject;
class ManagedObjectCondition;

class Manager
{
public:

	class Iterator
	{
	public:
		Iterator(ManagedObject* head);
		Iterator(const Iterator& iter);
		Iterator& operator=(const Iterator& iter);
		~Iterator();

		ManagedObject* operator++();
		ManagedObject* operator++(int);
		ManagedObject* operator->() const;
		ManagedObject& operator*();
		const bool Is_At_End() const;

	private:
		Iterator();

	private:
		ManagedObject* curr;
	};


	virtual ~Manager();


	void Destroy_Objects();
	void Print_Statistics(const char* preamble = "") const;

	const Iterator Active_Iterator() const;
		
	void Remove(ManagedObject* const obj);

protected:
	ManagedObject* Add_Object();
	ManagedObject* Find_Object(const ManagedObjectCondition& condition) const;




protected:
	Manager(const uint32_t refill);
	void Init(uint32_t initial);

	virtual ManagedObject* Make_Object() const abstract;
	virtual void Delete_Object(ManagedObject* obj) const abstract;
		
		



private:
	Manager();
	Manager(const Manager& manager);
	Manager& operator=(const Manager& manager);
		
	void Fill_Reserve(const uint32_t numToAllocate);

private:
	uint32_t refillSize;

	uint32_t activeCount;
	uint32_t maxActiveCount;

	uint32_t reserveCount;

	ManagedObject* activeHead;
	ManagedObject* reserveHead;
};
