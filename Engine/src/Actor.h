#pragma once

#include <PCSNode.h>
#include <MathEngine.h>
#include <ManagedObject.h>

#include "Model.h"
#include "ReferencedObject.h"

class UpdateStrategy;
class Material;

class Actor : public PCSNode, public ManagedObject, public ReferencedObject
{
public:
	void Set( ID3D11Device* device, const Material* material, ModelBase* modelBase, UpdateStrategy* strategy );
	virtual void Reset();

	void Update( uint32_t time );

	void Update_Model_Matrix();
	Model& Get_Model();


protected:
	virtual void Free_Me();

private:
	// Only the actor manager may create or destroy actors
	Actor();
	virtual ~Actor();

	friend class ActorManager;

	// Copying not allowed!
	Actor( const Actor& actor );
	Actor& operator=( const Actor& actor );

private:
	Model* model;
	UpdateStrategy* updateStrategy;

public:
	Vect position;
	Vect scale;
	Vect rotation;
};