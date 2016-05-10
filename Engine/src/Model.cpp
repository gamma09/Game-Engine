#include <d3d11_1.h>

#include "Model.h"
#include "ModelBase.h"
#include "MemorySetup.h"

#include "DirectXAssert.h"
#include "Animation.h"
#include "Camera.h"
#include "Texture.h"

Model::Model( ID3D11Device* device, const Material* inMaterial, ModelBase* baseModel )
	: ModelBaseInstance( baseModel ),
	activeTexture( 0 ),
	world( IDENTITY ),
	currentAnim( -1 ),
	currentAnimDuration( 0 ),
	currentAnimStartFrame( 0 ),
	maxSize( 1.0f ),
	materialsHead( nullptr )
{
	this->Add_Material( inMaterial );

	this->boneMatricesAllocationPointer = newArray( Matrix, this->baseModel->Get_Bone_Count() + 1, AnimHeap::Instance(), ALIGN_16 );

	// Adjust bone matrices so that boneMatrices[-1] is valid -> the matrix for bone -1 is the Identity matrix
	this->boneMatrices = this->boneMatricesAllocationPointer + 1;
	this->boneMatrices[-1].set( IDENTITY );

	this->Start_Animation( 0, 0 );
	this->Update_Animation( 0 );
	this->Stop_Animation();
	
	D3D11_BUFFER_DESC bd =
	{
		sizeof(Matrix),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_CONSTANT_BUFFER,
		0,
		0,
		0
	};

	GameCheckFatalDx( device->CreateBuffer( &bd, NULL, &this->worldBuffer ), "Cannot create world buffer." );

	D3D11_BUFFER_DESC bd2 =
	{
		sizeof( Matrix ) * this->baseModel->Get_Bone_Count(),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_CONSTANT_BUFFER,
		0,
		0,
		0
	};

	GameCheckFatalDx( device->CreateBuffer( &bd2, NULL, &this->bonesBuffer ), "Cannot create bones buffer." );
}

Model::~Model()
{
	this->worldBuffer->Release();
	this->bonesBuffer->Release();

	delete this->boneMatricesAllocationPointer;
	this->boneMatrices = nullptr;
	this->boneMatricesAllocationPointer = nullptr;

	this->baseModel->Remove_Reference();
	this->baseModel = nullptr;
	while( this->materialsHead )
	{
		this->Remove_Material( this->materialsHead->material );
	}
}

void Model::Start_Animation( const uint32_t time, const uint32_t animationID )
{
	this->currentAnimStartFrame = time;
	this->currentAnim = animationID;
	this->currentAnimDuration = this->baseModel->Get_Animation( animationID ).Get_Animation_Length();
}

void Model::Update_Animation( const uint32_t time )
{
	if( this->currentAnim != -1 )
	{
		uint32_t delta = time - this->currentAnimStartFrame;
		while( delta > this->currentAnimDuration )
		{
			delta -= this->currentAnimDuration;
			this->currentAnimStartFrame += this->currentAnimDuration;
		}

		const uint32_t boneCount = this->baseModel->Get_Bone_Count();

		for( unsigned int i = 0; i < boneCount; i++ )
		{
			int parentIndex = this->baseModel->Get_Parent_Bone_Of_Bone( i );
			GameAssert( -1 <= parentIndex && parentIndex < static_cast<int>( i ) );

			this->boneMatrices[i] = this->baseModel->Get_Bone_Transform( this->currentAnim, i, delta ) * this->boneMatrices[parentIndex];
			GameAssert( this->boneMatrices[i][m15] > 0.99f || this->boneMatrices[i][m15] < 1.01f );
		}
	}
}

bool Model::Should_Be_Drawn( const Material* material, const Camera* camera ) const
{
	return this->Has_Material( material ) &&
		camera->Should_Be_Drawn( Vect() * this->baseModel->Get_Bounding_Matrix() * this->world, this->maxSize * this->baseModel->Get_Bounding_Radius() );
}

#define WORLD_MATRIX_BUFFER_INDEX 3
#define BONE_MATRICES_BUFFER_INDEX 4
#define TEXTURE_VIEW_RESOURCE_INDEX 0

void Model::Draw( ID3D11DeviceContext* context, unsigned int triangleIndexCount, ID3DUserDefinedAnnotation* annotation ) const
{
	annotation->BeginEvent( L"Model::Draw()" );
	{
		Matrix* mtxData = newArray( Matrix, this->baseModel->Get_Bone_Count(), TemporaryHeap::Instance(), ALIGN_16 );
		memcpy( mtxData, this->boneMatrices, this->baseModel->Get_Bone_Count() * sizeof( Matrix ) );
		
		GameAssert( mtxData[1][m15] > 0.99f && mtxData[1][m15] < 1.01f );
		context->UpdateSubresource( this->bonesBuffer, 0, nullptr, mtxData, 0, 0 );

		delete mtxData;

		/*D3D11_MAPPED_SUBRESOURCE mappedModelBuffer;
		GameCheckFatalDx( context->Map( this->modelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedModelBuffer ), "Could not map model buffer." );
		memcpy( mappedModelBuffer.pData, &this->world, sizeof( Matrix ) );
		memcpy( reinterpret_cast<char*>( mappedModelBuffer.pData ) + sizeof( Matrix ), this->boneMatrices, baseModel->Get_Bone_Count() * sizeof( Matrix ) );
		context->Unmap( this->modelBuffer, 0 );*/

		context->VSSetConstantBuffers( BONE_MATRICES_BUFFER_INDEX, 1, &this->bonesBuffer );

		context->UpdateSubresource( this->worldBuffer, 0, nullptr, &this->world, 0, 0 );
		context->VSSetConstantBuffers( WORLD_MATRIX_BUFFER_INDEX, 1, &this->worldBuffer );

		ID3D11ShaderResourceView* textureResourceView = this->Get_Texture()->Get_Texture_Resource();
		context->PSSetShaderResources( TEXTURE_VIEW_RESOURCE_INDEX, 1, &textureResourceView );

		context->DrawIndexed( triangleIndexCount, 0, 0 );
	}
	annotation->EndEvent();
}

void Model::Add_Material( const Material* material )
{
	GameAssert( !this->Has_Material( material ) );

	this->materialsHead = new( ModelHeap::Instance(), ALIGN_4 ) MaterialLink( this->materialsHead, material );
}

bool Model::Has_Material( const Material* inMaterial ) const
{
	bool hasMaterial = false;
	for( const MaterialLink* curr = this->materialsHead; curr && !hasMaterial; curr = curr->next )
	{
		hasMaterial = ( curr->material == inMaterial );
	}

	return hasMaterial;
}

void Model::Remove_Material( const Material* material )
{
	MaterialLink* prev = nullptr;
	MaterialLink* curr = this->materialsHead;
	while( curr && curr->material != material )
	{
		prev = curr;
		curr = curr->next;
	}

	GameAssert( curr );
	if( prev )
	{
		prev->next = curr->next;
	}
	else
	{
		this->materialsHead = curr->next;
	}

	delete curr;
}