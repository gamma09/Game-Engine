#include "AnimatingStrategy.h"
#include "Actor.h"

AnimatingStrategy::AnimatingStrategy() :
	UpdateStrategy()
{
	// Do nothing
}

AnimatingStrategy::~AnimatingStrategy()
{
	// Do nothing
}

void AnimatingStrategy::Update(Actor& actor, uint32_t time)
{
	actor.Get_Model().Update_Animation(time);
}

void AnimatingStrategy::Free_Me()
{
	delete this;
}