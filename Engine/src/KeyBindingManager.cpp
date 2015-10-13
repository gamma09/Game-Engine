#include "KeyBindingManager.h"

#include <cassert>

#include "MemorySetup.h"
#include "KeyBinding.h"

KeyBindingManager* KeyBindingManager::instance;

KeyBindingManager* KeyBindingManager::Instance()
{
	assert(instance != 0);

	return instance;
}
		
void KeyBindingManager::Create(Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize)
{
	assert(instance == 0);
	managerHeap;
	instance = new(managerHeap, ALIGN_4) KeyBindingManager(initialReserve, refillSize);
}

void KeyBindingManager::Destroy()
{
	assert(instance != 0);

	instance->Destroy_Objects();
	MemReturnCode status = Mem::destroyHeap(instance->heap);
	status;
	assert(status == Mem_OK);
	delete instance;
	instance = 0;
}

KeyBinding* KeyBindingManager::Add(const int virtualKey, KeyAction* action)
{
	KeyBinding* keyBind = static_cast<KeyBinding*>(this->Add_Object());
	keyBind->Set(virtualKey, action);

	return keyBind;
}

void KeyBindingManager::Check_Input() const
{
	Manager::Iterator it = this->Active_Iterator();
	while (!it.Is_At_End())
	{
		static_cast<KeyBinding*>(&*it)->Check_Key();
		it++;
	}
}

ManagedObject* KeyBindingManager::Make_Object() const
{
	return new KeyBinding();
}

void KeyBindingManager::Delete_Object(ManagedObject* obj) const
{
	delete obj;
}

KeyBindingManager::KeyBindingManager(const uint32_t initialReserve, const uint32_t refillSize) :
	Manager(refillSize)
{
	// Hey, might as well allocate a whole page of memory...
	MemReturnCode status = Mem::createHeap(this->heap, 4096, "Keybinding Heap");
	status;
	assert(status == Mem_OK);

	this->Init(initialReserve);
}

KeyBindingManager::~KeyBindingManager()
{
	// Do nothing
}