#include "KeyBindingManager.h"

#include <GameAssert.h>

#include "MemorySetup.h"
#include "KeyBinding.h"

KeyBindingManager* KeyBindingManager::instance = nullptr;

KeyBindingManager* KeyBindingManager::Instance()
{
	GameAssert( instance );
	return instance;
}

void KeyBindingManager::Create( Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize )
{
	GameAssert( instance == 0 );
	instance = new( managerHeap, ALIGN_4 ) KeyBindingManager( initialReserve, refillSize );
}

void KeyBindingManager::Destroy()
{
	GameAssert( instance );
	instance->PreDestroy();
	delete instance;
	instance = nullptr;
}

KeyBinding* KeyBindingManager::Add( const int virtualKey, KeyAction* action )
{
	KeyBinding* keyBind = static_cast<KeyBinding*>( this->Add_Object() );
	keyBind->Set( virtualKey, action );
	return keyBind;
}

void KeyBindingManager::Check_Input() const
{
	Manager::Iterator it = this->Active_Iterator();
	while( !it.Is_At_End() )
	{
		static_cast<KeyBinding*>( &*it )->Check_Key();
		it++;
	}
}

ManagedObject* KeyBindingManager::Make_Object() const
{
	return new KeyBinding();
}

void KeyBindingManager::Delete_Object( ManagedObject* obj ) const
{
	delete obj;
}

KeyBindingManager::KeyBindingManager( const uint32_t initialReserve, const uint32_t refillSize )
	: Manager( refillSize )
{
	// Hey, might as well allocate a whole page of memory...
	Mem::createVariableBlockHeap( this->heap, 4096, "Key Bindings" );
	this->Init( initialReserve );
}

KeyBindingManager::~KeyBindingManager()
{
	Mem::destroyHeap( heap );
}