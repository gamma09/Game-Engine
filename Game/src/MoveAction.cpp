#include "MoveAction.h"

#include "Moveable.h"

MoveAction::MoveAction(Moveable* target, const float dx, const float dy, const float dz) :
	vel(dx, dy, dz),
	target(target)
{
	// Do nothing
}

MoveAction::~MoveAction()
{
	// Do nothing
}

void MoveAction::On_Key_Down()
{
	target->Add_Velocity(this->vel);
}

void MoveAction::On_Key_Up()
{
	target->Add_Velocity(-(this->vel));
}

void MoveAction::Free_Me()
{
	delete this;
}