#pragma once

#include "ReferencedObject.h"

class KeyAction : public ReferencedObject
{
public:
	virtual void On_Key_Down() = 0;
	virtual void On_Key_Up() = 0;

protected:
	KeyAction() { }
	KeyAction(const KeyAction&) { }
	KeyAction& operator=(const KeyAction&) { }
	virtual ~KeyAction() { }
};