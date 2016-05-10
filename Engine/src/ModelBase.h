#pragma once

#include <stdint.h>

#include <ManagedObject.h>
#include <Matrix.h>

#include "ReferencedObject.h"

class Animation;
class Camera;
class Material;
class Model;
class ModelBaseInstance;
class ModelBaseManager;
class Texture;

struct Header;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3DUserDefinedAnnotation;





class ModelBase : public ManagedObject, public ReferencedObject
{
public:

	// Creates a model based on the specified file
	void Set( ID3D11Device* device, const char* const archiveFile );
	
	virtual void Reset() override;

	virtual void Draw( ID3D11DeviceContext* context, const Material* material, const Camera* camera, ID3DUserDefinedAnnotation* annotation ) const;

	const Texture* Get_Texture( uint32_t textureID ) const;
	const Matrix& Get_Bounding_Matrix() const;
	float Get_Bounding_Radius() const;
	Matrix Get_Bone_Transform( uint32_t animID, uint32_t boneIndex, uint32_t time ) const;
	const Animation& Get_Animation( uint32_t animID ) const;
	uint32_t Get_Bone_Count() const;
	int Get_Parent_Bone_Of_Bone( unsigned int boneIndex ) const;

	Model* Create_Instance( ID3D11Device* device, const Material* material );
	void Delete_Instance( ModelBaseInstance* instance );

protected:
	virtual void Free_Me() override;


private:
	// Bounding sphere
	Matrix boundsMatrix;
	float boundingRadius;

	// Vertices
	ID3D11Buffer* verticesBuffer;
	// Triangles
	ID3D11Buffer* indicesBuffer;
	uint32_t triangleVertexCount;

	// Hierarchy
	uint32_t boneCount;
	int* boneParentList;
	ID3D11Buffer* bindMatricesBuffer;
	
	// Animations
	uint32_t animCount;
	Animation* anims;

	// Textures
	uint32_t textureCount;
	Texture* textureHead;

	ModelBaseInstance* instancesHead;

private:
	ModelBase();
	virtual ~ModelBase();

	void Read_Hierarchy( ID3D11Device* device, const Header& header, unsigned char*& modelData );
	void Read_Animations( const Header& header, unsigned char*& modelData );
	void Read_Vertices( ID3D11Device* device, const Header& header, unsigned char*& modelData );
	void Read_Triangles( ID3D11Device* device, const Header& header, unsigned char*& modelData );
	void Read_Textures( ID3D11Device* device, const Header& header, unsigned char*& modelData, const char* const archiveFile );


	ModelBase( const ModelBase& model ) = delete;
	ModelBase& operator=( const ModelBase& model ) = delete;


	friend class ModelBaseManager;
};

class ModelBaseInstance
{
protected:
	ModelBaseInstance( ModelBase* baseModel );
	virtual ~ModelBaseInstance();

	virtual bool Should_Be_Drawn( const Material* material, const Camera* camera ) const abstract;
	virtual void Draw( ID3D11DeviceContext* context, unsigned int triangleIndexCount, ID3DUserDefinedAnnotation* annotation ) const abstract;

private:
	ModelBaseInstance* next;
	ModelBaseInstance* prev;

protected:
	ModelBase* baseModel;



private:
	ModelBaseInstance( const ModelBaseInstance& instance ) = delete;
	ModelBaseInstance& operator=( const ModelBaseInstance& instance ) = delete;
	friend class ModelBase;
};