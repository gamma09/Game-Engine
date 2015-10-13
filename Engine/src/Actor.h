#pragma once

#include <PCSNode.h>
#include <MathEngine.h>
#include <ManagedObject.h>

#include "Model.h"
#include "ReferencedObject.h"

class UpdateStrategy;
struct DrawInfo;

class Actor : public PCSNode, public ManagedObject, public ReferencedObject
{
public:
	void Set(ModelBase* modelBase, UpdateStrategy* strategy);
	virtual void Reset();

	void Update(const uint32_t& timeDelta);
	const bool Draw(DrawInfo& info) const;

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
	Actor(const Actor& actor);
	Actor& operator=(const Actor& actor);

private:
	Model model;
	UpdateStrategy* updateStrategy;

public:
	Vect position;

	// rotation about the X axis
	float rx;
	// rotation about the Y axis
	float ry;
	// rotation about the Z axis
	float rz;

	// scale along X axis
	float sx;
	// scale along Y axis
	float sy;
	// scale along Z axis
	float sz;

private:
	float maxSize;
};