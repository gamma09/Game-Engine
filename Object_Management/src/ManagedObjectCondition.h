#pragma once

class ManagedObject;



class ManagedObjectCondition
{
public:
	ManagedObjectCondition() { }
	ManagedObjectCondition(const ManagedObjectCondition&) { }
	ManagedObjectCondition& operator=(const ManagedObjectCondition&) { }
	virtual ~ManagedObjectCondition() { }

	virtual const bool Matches(const ManagedObject& obj) const abstract;
};