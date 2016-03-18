#pragma once

#include "UpdateStrategy.h"

class StationaryStrategy : public UpdateStrategy
{
public:
	StationaryStrategy();
	virtual ~StationaryStrategy();

	virtual void Update(Actor& actor, uint32_t time) override;

protected:
	virtual void Free_Me() override;

private:
	StationaryStrategy(const StationaryStrategy& strategy);
	StationaryStrategy& operator=(const StationaryStrategy& strategy);
};