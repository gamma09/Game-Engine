#pragma once

class Manager;
class ManagedObjectCondition;




class ManagedObject
{
protected:
	ManagedObject();

public:
	ManagedObject(const ManagedObject& obj);
	ManagedObject& operator=(const ManagedObject& obj);
	virtual ~ManagedObject();
	
	virtual void Reset();

private:
	ManagedObject* next;
	ManagedObject* prev;
	bool isActive;

#ifdef _DEBUG
#define MAX_ID_LENGTH 16

	char ID[MAX_ID_LENGTH];
#endif

	friend class Manager;
};