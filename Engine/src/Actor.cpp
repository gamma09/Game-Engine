#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GameAssert.h>

#include "Actor.h"
#include "Camera.h"
#include "UpdateStrategy.h"
#include "ActorManager.h"
#include "ModelBase.h"
#include "DrawInfo.h"

Actor::Actor()
	: PCSNode(),
	ManagedObject(),
	ReferencedObject(),
	model()
{
}

void Actor::Set( Material* material, ModelBase* modelBase, UpdateStrategy* updateStrategy )
{
	GameAssert( modelBase != 0 );
	GameAssert( updateStrategy != 0 );

	this->position.set( 0.0f, 0.0f, 0.0f );
	this->rx = 0.0f;
	this->ry = 0.0f;
	this->rz = 0.0f;
	this->sx = 1.0f;
	this->sy = 1.0f;
	this->sz = 1.0f;
	this->model.Set( material, modelBase );
	this->updateStrategy = updateStrategy;
	this->updateStrategy->Add_Reference();

	this->Update_Model_Matrix();

}

void Actor::Reset()
{
	GameAssert( this->Get_Reference_Count() == 0 );

	this->model.Reset();
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

void Actor::Update( const uint32_t& currentTime )
{
	this->updateStrategy->Update( *this, currentTime );
}

Model& Actor::Get_Model()
{
	return this->model;
}

bool Actor::Draw( DrawInfo& info ) const
{
	if( info.camera->Should_Be_Drawn( Vect() * this->model.baseModel->Get_Bounding_Matrix() * this->model.world, this->maxSize * this->model.baseModel->Get_Bounding_Radius() ) )
	{
		this->model.Draw( info );
		return true;
	}
	else
		return false;
}

void Actor::Update_Model_Matrix()
{
	Matrix transform( SCALE, this->sx, this->sy, this->sz );
	transform *= Matrix( ROT_X, this->rx );
	transform *= Matrix( ROT_Y, this->ry );
	transform *= Matrix( ROT_Z, this->rz );
	transform *= Matrix( TRANS, this->position );

	this->model.Set_World( transform );

	this->maxSize = max( max( this->sx, this->sy ), this->sz );
}