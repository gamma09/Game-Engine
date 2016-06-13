#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GameAssert.h>

#include "Actor.h"
#include "Camera.h"
#include "UpdateStrategy.h"
#include "ActorManager.h"
#include "ModelBase.h"
#include "Material.h"

Actor::Actor()
	: PCSNode(),
	ManagedObject(),
	ReferencedObject(),
	model( nullptr )
{
}

void Actor::Set( ID3D11Device* device, const Material* material, ModelBase* modelBase, UpdateStrategy* updateStrategy )
{
	GameAssert( modelBase != 0 );
	GameAssert( updateStrategy != 0 );

	this->position.set( 0.0f, 0.0f, 0.0f );
	this->rotation.set( 0.0f, 0.0f, 0.0f );
	this->scale.set( 1.0f, 1.0f, 1.0f );
	this->model = modelBase->Create_Instance( device, material );
	this->updateStrategy = updateStrategy;
	this->updateStrategy->Add_Reference();

	this->Update_Model_Matrix();

}

void Actor::Reset()
{
	GameAssert( this->Get_Reference_Count() == 0 );

	this->model->Delete_Instance();
	this->updateStrategy->Remove_Reference();
	this->updateStrategy = 0;
}

Actor::~Actor()
{
	// Do nothing
}

void Actor::Free_Me()
{
	ActorManager::Instance()->Remove( this );
}

void Actor::Update( uint32_t currentTime )
{
	this->updateStrategy->Update( *this, currentTime );
}

Model& Actor::Get_Model()
{
	return *this->model;
}

void Actor::Update_Model_Matrix()
{
	Matrix transform( SCALE, this->scale );
	transform *= Matrix( ROT_X, this->rotation[X] );
	transform *= Matrix( ROT_Y, this->rotation[Y] );
	transform *= Matrix( ROT_Z, this->rotation[Z] );
	transform *= Matrix( TRANS, this->position );

	this->model->Set_World( transform );
	this->model->Set_Max_Size( max( max( this->scale[X], this->scale[Y] ), this->scale[Z] ) );
}