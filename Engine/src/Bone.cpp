#include <Quat.h>

#include "DebuggerSetup.h"
#include "Bone.h"
#include "ModelBase.h"
#include "DrawInfo.h"
#include "BoneManager.h"

void Bone::Set( const uint32_t& meshIndex )
{
	this->worldMatrix.set( IDENTITY );
	this->boneMatrix.set( IDENTITY );
	this->meshIndex = meshIndex;
}

void Bone::Reset()
{
	this->setParent( 0 );

	Bone* child = static_cast<Bone*>( this->getChild() );
	if( child != 0 )
		BoneManager::Instance()->Remove( child );

	this->setChild( 0 );

	Bone* sibling = static_cast<Bone*>( this->getSibling() );
	if( sibling != 0 )
		BoneManager::Instance()->Remove( sibling );

	this->setSibling( 0 );
}

const Matrix& Bone::Get_Matrix() const
{
	return this->worldMatrix;
}

void Bone::Set_Frame( const ModelBase* const model, const uint32_t& animationID, const uint32_t& frameTimeMillis )
{
	this->worldMatrix = model->Get_Bone_Transform( animationID, this->meshIndex, frameTimeMillis ) * static_cast<HierarchyElement*>( this->getParent() )->Get_Matrix();

	Bone* child = static_cast<Bone*>( this->getChild() );
	if( child != 0 )
		child->Set_Frame( model, animationID, frameTimeMillis );

	Bone* sibling = static_cast<Bone*>( this->getSibling() );
	if( sibling != 0 )
		sibling->Set_Frame( model, animationID, frameTimeMillis );
}

void Bone::Draw( DrawInfo& info, const Material& material ) const
{
	info.boneIndex = this->meshIndex;
	// TODO change this eventually (with skinning)
	//info.worldMatrix = &this->worldMatrix;

	Matrix mtx = this->boneMatrix * this->worldMatrix;
	info.worldMatrix = &mtx;

	material.Draw( info );

	Bone* child = static_cast<Bone*>( this->getChild() );
	if( child != 0 )
		child->Draw( info, material );

	Bone* sibling = static_cast<Bone*>( this->getSibling() );
	if( sibling != 0 )
		sibling->Draw( info, material );
}

#define BONE_MARKER_SIZE 5.0f

void Bone::Bind_Pose( bool isFirstGeneration )
{
	Matrix scale( SCALE, BONE_MARKER_SIZE, BONE_MARKER_SIZE, BONE_MARKER_SIZE );
	this->boneMatrix = scale;

	Bone* child = static_cast<Bone*>( this->getChild() );
	if( child != 0 )
		child->Bind_Pose( false );

	Bone* sibling = static_cast<Bone*>( this->getSibling() );
	if( sibling != 0 )
		sibling->Bind_Pose( isFirstGeneration );
}




Bone::Bone()
	: HierarchyElement(),
	ManagedObject()
{
	// Do nothing
}

Bone::~Bone()
{
	// Do nothing
}