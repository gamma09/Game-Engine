#pragma once

#include <MathEngine.h>

#include "ModelBase.h"

class Material;
class Texture;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3DUserDefinedAnnotation;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;

struct MaterialLink
{
	MaterialLink( MaterialLink* inNext, const Material* inMaterial ) : next( inNext ), material( inMaterial ) {}
	MaterialLink( const MaterialLink& ) = delete;
	MaterialLink& operator=( const MaterialLink& ) = delete;
	~MaterialLink() {}

	MaterialLink* next;
	const Material* material;
};

class Model : public ModelBaseInstance
{
public:

	inline void Change_Active_Texture( uint32_t activeTexture ) { this->activeTexture = activeTexture; }
	inline const Texture* Get_Texture() const { return this->baseModel->Get_Texture( this->activeTexture ); }

	inline void Set_World( const Matrix& world ) { this->world = world; }
	inline const Matrix& Get_World() const { return this->world; }

	inline void Set_Max_Size( float inMaxSize ) { this->maxSize = inMaxSize; }
	
	inline const ModelBase* Get_Base_Model()    const { return this->baseModel; }

	void Start_Animation( uint32_t time, uint32_t animationID );
	void Update_Animation( uint32_t time );
	inline void Delete_Instance() { this->baseModel->Delete_Instance( this ); }

	inline void Stop_Animation() { this->currentAnim = -1; }

	virtual bool Should_Be_Drawn( const Material* material, const Camera* camera ) const override;
	virtual void Draw( ID3D11DeviceContext* context, unsigned int triangleIndexCount, ID3DUserDefinedAnnotation* annotation ) const override;
	virtual void Update_Skeleton( ID3D11DeviceContext* context, ID3DUserDefinedAnnotation* annotation ) const override;
	
	void Add_Material( const Material* material );
	bool Has_Material( const Material* material ) const;
	void Remove_Material( const Material* material );

private:
	// Only ModelBase is allowed to create or destroy models
	Model( ID3D11Device* device, const Material* material, ModelBase* baseModel );
	virtual ~Model();

	Model( const Model& model ) = delete;
	Model& operator=( const Model& model ) = delete;
	

private:
	// working in conjunction with ModelBaseInstance, this pointer will put the world matrix on a 16-byte aligned boundary
	MaterialLink* materialsHead;
	Matrix world;
	float maxSize;

	ID3D11Buffer* modelBuffer;

	ID3D11Buffer* boneMatricesBuffer;
	// Used as output from compute shader
	ID3D11UnorderedAccessView* boneMatricesUAV;
	// Used as input to the vertex shader
	ID3D11ShaderResourceView* boneMatricesSRV;

	uint32_t activeTexture;

	
	uint32_t currentAnimStartFrame;
	uint32_t currentAnimDuration;
	int32_t currentAnim;

	uint32_t animationTime;

	// ModelBase is in charge of managi
	friend class ModelBase;
};

