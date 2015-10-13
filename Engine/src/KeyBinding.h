#pragma once

#include <ManagedObject.h>
#include "KeyAction.h"

class KeyBinding : ManagedObject
{
public:

	void Set(const int virtualKey, KeyAction* action);
	virtual void Reset();

	void Check_Key();

private:
	// May only be created or destroyed by the KeyBindingManager
	KeyBinding();
	virtual ~KeyBinding();

	// May not be copied
	KeyBinding(const KeyBinding& binding);
	KeyBinding& operator=(const KeyBinding& binding);
	

	friend class KeyBindingManager;


private:
	int virtualKey;
	bool wasDown;
	KeyAction* action;
};