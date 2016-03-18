#pragma once

#include "UpdateStrategy.h"

class AnimatingStrategy : public UpdateStrategy
{
public:
	AnimatingStrategy();
	virtual ~AnimatingStrategy();

	virtual void Update(Actor& actor, uint32_t time) override;

protected:
	virtual void Free_Me() override;

private:
	AnimatingStrategy(const AnimatingStrategy& strategy);
	AnimatingStrategy& operator=(const AnimatingStrategy& strategy);
};