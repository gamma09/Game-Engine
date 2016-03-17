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
struct DrawInfo;
struct ID3D11Device;

class ModelBase : public ManagedObject, public ReferencedObject
{
public:

	// Creates a model based on the specified file
	void Set( ID3D11Device* device, const char* const archiveFile );
	
	virtual void Reset() override;

	Bone* const Create_Skeleton_From_Model() const;

	virtual void Draw( const DrawInfo& info ) const;

	const Texture* Get_Texture( uint32_t textureID ) const;
	const Matrix& Get_Bounding_Matrix() const;
	float Get_Bounding_Radius() const;
	Matrix Get_Bone_Transform( uint32_t animID, uint32_t boneIndex, uint32_t time ) const;
	const Animation& Get_Animation( uint32_t animID ) const;

protected:
	virtual void Free_Me() override;


private:
	Matrix boundsMatrix;
	float boundingRadius;

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


	ModelBase( const ModelBase& model );
	ModelBase& operator=( const ModelBase& model );


	friend class ModelBaseManager;
};