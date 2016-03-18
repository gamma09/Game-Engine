#include "StationaryStrategy.h"

StationaryStrategy::StationaryStrategy()
{
	// Do nothing
}

StationaryStrategy::~StationaryStrategy()
{
	// Do nothing
}

void StationaryStrategy::Update(Actor&, uint32_t)
{
	// Do nothing
}

void StationaryStrategy::Free_Me()
{
	delete this;
}