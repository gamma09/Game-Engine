#pragma once

#include <cstdint>


#include <ManagedObject.h>
#include <Matrix.h>

#include "ReferencedObject.h"

class Animation;
struct Mesh;
class ModelBaseManager;
class Texture;
class Bone;

class ModelBase : public ManagedObject, public ReferencedObject
{
public:
	

	void Set(const char* const archiveFile);
	virtual void Reset() override;

	Bone* const Create_Skeleton_From_Model() const;

	virtual void Draw(const uint32_t meshIndex) const;

	const Texture* Get_Texture(const uint32_t textureID) const;
	const Matrix& Get_Bounding_Matrix() const;
	const float Get_Bounding_Radius() const;
	const Matrix Get_Bone_Transform(const uint32_t animID, const uint32_t boneIndex, const uint32_t time) const;
	const Animation& Get_Animation(const uint32_t animID) const;

protected:
	virtual void Free_Me() override;


private:
	float boundingRadius;
	Matrix boundsMatrix;

	uint32_t boneCount;
	Mesh* boneMeshes;

	uint32_t animCount;
	Animation* anims;

	uint32_t textureCount;
	Texture* textureHead;

	int* boneParentList;

private:
	ModelBase();
	virtual ~ModelBase();


	ModelBase(const ModelBase& model);
	ModelBase& operator=(const ModelBase& model);


	friend class ModelBaseManager;
};