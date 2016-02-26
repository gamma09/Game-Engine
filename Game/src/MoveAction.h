#pragma once

#include "KeyAction.h"
#include <Vect.h>

class Moveable;

class MoveAction : public KeyAction
{
public:
	MoveAction(Moveable* target, const float dx, const float dy, const float dz);
	virtual ~MoveAction();

	virtual void On_Key_Down() override;
	virtual void On_Key_Up() override;

protected:
	virtual void Free_Me() override;


private:
	MoveAction(const MoveAction& action);
	MoveAction& operator=(const MoveAction& action);



private:
	Moveable* target;
	Vect vel;
};