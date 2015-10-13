#pragma once

#include <Manager.h>

class Heap;
class KeyBinding;
class KeyAction;

class KeyBindingManager : public Manager
{
public:

	static KeyBindingManager* Instance();
		
	static void Create(Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize);
	static void Destroy();

	KeyBinding* Add(const int virtualKey, KeyAction* action);
	void Check_Input() const;

protected:
	virtual ManagedObject* Make_Object() const;
	virtual void Delete_Object(ManagedObject* obj) const;

private:
	KeyBindingManager(const uint32_t initialReserve, const uint32_t refillSize);
	virtual ~KeyBindingManager();



	KeyBindingManager();
	KeyBindingManager(const KeyBindingManager& manager);
	KeyBindingManager& operator=(const KeyBindingManager& manager);


private:
	Heap* heap;

	static KeyBindingManager* instance;
};