#include "KeyBinding.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void KeyBinding::Set(const int virtualKey, KeyAction* action)
{
	this->virtualKey = virtualKey;
	this->action = action;
	this->action->Add_Reference();
	this->wasDown = false;
}

void KeyBinding::Reset()
{
	ManagedObject::Reset();

	this->action->Remove_Reference();
	this->action = 0;
	this->virtualKey = 0;
}

void KeyBinding::Check_Key()
{
	short isDown = GetKeyState(this->virtualKey) & 0x8000;
	if (isDown && !this->wasDown)
	{
		this->action->On_Key_Down();
		this->wasDown = isDown != 0;
	}
	else if (this->wasDown && !isDown)
	{
		this->action->On_Key_Up();
		this->wasDown = isDown != 0;
	}
}

KeyBinding::KeyBinding() :
	ManagedObject(),
	action(0),
	virtualKey(0),
	wasDown(false)
{
	// Do nothing
}

KeyBinding::~KeyBinding()
{
	// Do nothing
}