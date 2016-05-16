#include <d3d11_1.h>
#include <DebuggerSetup.h>

#include "Model.h"
#include "ModelBase.h"
#include "MemorySetup.h"

#include "DirectXAssert.h"
#include "Animation.h"
#include "Camera.h"
#include "Texture.h"
#include "SharedShaderDefines.h"

Model::Model( ID3D11Device* device, const Material* inMaterial, ModelBase* baseModel )
	: ModelBaseInstance( baseModel ),
	activeTexture( 0 ),
	world( IDENTITY ),
	currentAnim( -1 ),
	currentAnimDuration( 0 ),
	currentAnimStartFrame( 0 ),
	maxSize( 1.0f ),
	materialsHead( nullptr ),
	animationTime( 0 )
{
	this->Add_Material( inMaterial );

	D3D11_BUFFER_DESC modelBufferDesc ={ sizeof( Matrix ) + 4 * sizeof( float ), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
	GameCheckFatalDx( device->CreateBuffer( &modelBufferDesc, nullptr, &this->modelBuffer ), "Could not create model's world/interpolation time buffer." );

	D3D11_BUFFER_DESC boneMatricesBufferDesc =
	{
		sizeof( Matrix ) * MAX_BONES,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		0,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof( Matrix )
	};

	GameCheckFatalDx( device->CreateBuffer( &boneMatricesBufferDesc, nullptr, &this->boneMatricesBuffer ), "Could not create buffer to pass model's bones from CS to VS." );

	D3D11_UNORDERED_ACCESS_VIEW_DESC boneMatricesUAVDesc;
	boneMatricesUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	boneMatricesUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	boneMatricesUAVDesc.Buffer.FirstElement = 0;
	boneMatricesUAVDesc.Buffer.NumElements = MAX_BONES;
	boneMatricesUAVDesc.Buffer.Flags = 0;

	GameCheckFatalDx( device->CreateUnorderedAccessView( this->boneMatricesBuffer, &boneMatricesUAVDesc, &this->boneMatricesUAV ), "Couldn't create UAV for bone matrices buffer." );

	D3D11_SHADER_RESOURCE_VIEW_DESC boneMatricesSRVDesc;
	boneMatricesSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	boneMatricesSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	boneMatricesSRVDesc.BufferEx.FirstElement = 0;
	boneMatricesSRVDesc.BufferEx.NumElements = MAX_BONES;
	boneMatricesSRVDesc.BufferEx.Flags = 0;

	GameCheckFatalDx( device->CreateShaderResourceView( this->boneMatricesBuffer, &boneMatricesSRVDesc, &this->boneMatricesSRV ), "Couldn't create SRV for bone matrices buffer." );
}

Model::~Model()
{
	this->modelBuffer->Release();
	this->boneMatricesBuffer->Release();
	this->boneMatricesSRV->Release();
	this->boneMatricesUAV->Release();

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
	this->animationTime = 0;
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

		this->animationTime = delta;
	}
}

bool Model::Should_Be_Drawn( const Material* material, const Camera* camera ) const
{
	return this->Has_Material( material ) &&
		camera->Should_Be_Drawn( Vect() * this->baseModel->Get_Bounding_Matrix() * this->world, this->maxSize * this->baseModel->Get_Bounding_Radius() );
}

static ID3D11ShaderResourceView* NULL_SRV[1] ={ nullptr };

void Model::Draw( ID3D11DeviceContext* context, unsigned int triangleIndexCount, ID3DUserDefinedAnnotation* annotation ) const
{
	annotation->BeginEvent( L"Model::Draw()" );
	{
		context->VSSetShaderResources( VS_BONE_MATRICES_REGISTER, 1, &this->boneMatricesSRV );
		ID3D11ShaderResourceView* textureResourceView = this->Get_Texture()->Get_Texture_Resource();
		context->PSSetShaderResources( PS_DIFFUSE_TEXTURE_REGISTER, 1, &textureResourceView );

		context->DrawIndexed( triangleIndexCount, 0, 0 );

		context->VSSetShaderResources( VS_BONE_MATRICES_REGISTER, 1, NULL_SRV );
	}
	annotation->EndEvent();
}

struct ModelInfo
{
	Matrix worldMatrix;
	float interpTime;
	float padding[3];
};

static ID3D11UnorderedAccessView* NULL_UAV[1] ={ nullptr };

void Model::Update_Skeleton( ID3D11DeviceContext* context, ID3DUserDefinedAnnotation* annotation ) const
{
	
	annotation->BeginEvent( L"Model::Update_Skeleton()" );
	{
		if( this->currentAnim != -1 )
		{
			const Animation& currentAnimation = this->baseModel->Get_Animation( this->currentAnim );

			ModelInfo info;
			info.worldMatrix = this->world;
			info.interpTime = currentAnimation.Activate_Key_Frame_SRVs( context, this->animationTime );
			context->UpdateSubresource( this->modelBuffer, 0, nullptr, &info, 0, 0 );
			context->CSSetConstantBuffers( CS_MODEL_DATA_REGISTER, 1, &this->modelBuffer );
			
			context->CSSetUnorderedAccessViews( CS_OUTPUT_BONES_REGISTER, 1, &this->boneMatricesUAV, nullptr );

			context->Dispatch( 1, 1, 1 );

			// Unbind the UAV so that the corresponding SRV can be bound when we start drawing
			context->CSSetUnorderedAccessViews( CS_OUTPUT_BONES_REGISTER, 1, NULL_UAV, nullptr );
		}
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