#pragma once

#include <ManagedObject.h>
#include <Matrix.h>

#include "HierarchyElement.h"
#include "Material.h"

struct DrawInfo;
class ModelBase;

class Bone : public HierarchyElement, public ManagedObject
{
public:

	virtual void Set( const uint32_t& meshIndex );
	virtual void Reset() override;

	virtual const Matrix& Get_Matrix() const override;

	void Set_Frame( const ModelBase* const model, const uint32_t& animationID, const uint32_t& frameTimeMillis );
	void Draw( DrawInfo& info, const Material& material ) const;
	void Bind_Pose( bool isFirstGeneration );



private:
	Bone();
	virtual ~Bone() override;



	Bone( const Bone& b ) = delete;
	Bone& operator=( const Bone& b ) = delete;

	friend class BoneManager;

private:
	Matrix worldMatrix;
	Matrix boneMatrix;
	uint32_t meshIndex;
};