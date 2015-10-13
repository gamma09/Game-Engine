#pragma once

#include <cstdint>
#include "ReferencedObject.h"

class Actor;

class UpdateStrategy abstract : public ReferencedObject
{
public:
	UpdateStrategy() : ReferencedObject() { }
	virtual ~UpdateStrategy() { }

	virtual void Update(Actor& actor, const uint32_t& time) abstract;

private:
	UpdateStrategy(const UpdateStrategy& strategy);
	UpdateStrategy& operator=(const UpdateStrategy& strategy);
};