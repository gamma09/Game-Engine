#include <GameAssert.h>

#include <File.h>
#include <MathEngine.h>
#include <malloc.h>
#include <new>
#include <asset_reader.h>
#include <stdio.h>
#include <utility>
#include <d3d11_1.h>

#include "DirectXAssert.h"
#include "MemorySetup.h"
#include "Model.h"
#include "Camera.h"
#include "ModelBase.h"
#include "ModelBaseManager.h"
#include "Texture.h"
#include "TextureManager.h"
#include "GlobalHeaps.h"
#include "Animation.h"
#include "KeyFrame.h"

// Maximum number of bones that can influence a vertex - this corresponds to the constant in Input.hlsli by the same name
#define MAX_INFLUENCE_COUNT 4

ModelBase::ModelBase()
	: ManagedObject(),
	ReferencedObject(),
	boundsMatrix(),
	boundingRadius( 0.0f ),
	verticesBuffer( nullptr ),
	indicesBuffer( nullptr ),
	triangleVertexCount( 0 ),
	boneCount( 0 ),
	boneParentList( nullptr ),
	bindMatricesBuffer( nullptr ),
	animCount( 0 ),
	anims( nullptr ),
	textureCount( 0 ),
	textureHead( nullptr ),
	instancesHead( nullptr )
{
	// Do nothing
}

ModelBase::~ModelBase()
{
	// Do nothing
}

struct Header
{
	float radius;
	float boundsCenterX;
	float boundsCenterY;
	float boundsCenterZ;
	unsigned int vertexCount;
	unsigned int triangleCount;
	unsigned int textureCount;
	unsigned int boneCount;
	unsigned int animCount;
};

struct Vertex
{
	float x;
	float y;
	float z;
	float w;

	float u;
	float v;

	float normX;
	float normY;
	float normZ;
	float normW;

	float boneWeight[MAX_INFLUENCE_COUNT];
	unsigned int boneIndices[MAX_INFLUENCE_COUNT];
};



struct Triangle
{
	unsigned int vertexA;
	unsigned int vertexB;
	unsigned int vertexC;
};

const static Triangle cubeTriList[] =
{
	{ 0, 2, 1 }, // Good
	{ 1, 2, 3 }, // Good

	{ 4, 5, 6 }, // Good
	{ 7, 6, 5 }, // Good

	{ 8, 9, 10 }, // Good
	{ 9, 11, 10 }, // Good

	{ 12, 14, 13 }, // Good
	{ 13, 14, 15 }, // Good

	{ 16, 17, 18 }, // Good
	{ 17, 19, 18 }, // Good

	{ 20, 22, 21 }, // Good
	{ 21, 22, 23 } // Good
};

const static Vertex cubeData[] =
{
	// left face
	{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -0.6f, -0.6f, -0.6f, 0.0f },
	{ -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, -0.6f, -0.6f, 0.6f, 0.0f },
	{ -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -0.6f, 0.6f, -0.6f, 0.0f },
	{ -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, -0.6f, 0.6f, 0.6f, 0.0f },


	// right face
	{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.6f, -0.6f, -0.6f, 0.0f },
	{ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.6f, -0.6f, 0.6f, 0.0f },
	{ 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.6f, 0.6f, -0.6f, 0.0f },
	{ 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.6f, 0.6f, 0.6f, 0.0f },

	// bottom face
	{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -0.6f, -0.6f, -0.6f, 0.0f },
	{ -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, -0.6f, -0.6f, 0.6f, 0.0f },
	{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.6f, -0.6f, -0.6f, 0.0f },
	{ 0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.6f, -0.6f, 0.6f, 0.0f },

	// top face
	{ -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -0.6f, 0.6f, -0.6f, 0.0f },
	{ -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, -0.6f, 0.6f, 0.6f, 0.0f },
	{ 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.6f, 0.6f, -0.6f, 0.0f },
	{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.6f, 0.0f },

	// front face
	{ -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -0.6f, -0.6f, 0.6f, 0.0f },
	{ -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, -0.6f, 0.6f, 0.6f, 0.0f },
	{ 0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.6f, -0.6f, 0.6f, 0.0f },
	{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.6f, 0.0f },

	// back face
	{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -0.6f, -0.6f, -0.6f, 0.0f },
	{ -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, -0.6f, 0.6f, -0.6f, 0.0f },
	{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.6f, -0.6f, -0.6f, 0.0f },
	{ 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.6f, 0.6f, -0.6f, 0.0f }
};


void ModelBase::Read_Hierarchy( ID3D11Device* device, const Header& header, unsigned char*& modelData )
{
	this->boneCount = header.boneCount;
	this->boneParentList = newArray( int, header.boneCount, AnimHeap::Instance(), ALIGN_4 );
	Matrix* boneBindMatrices = newArray( Matrix, header.boneCount, AnimHeap::Instance(), ALIGN_16 );

	for( unsigned int i = 0; i < header.boneCount; i++ )
	{
		this->boneParentList[i] = *reinterpret_cast<int*>( modelData );
		modelData += sizeof( int );
		boneBindMatrices[i] = *reinterpret_cast<Matrix*>( modelData );
		modelData += sizeof( Matrix );
	}

	D3D11_BUFFER_DESC bindBufferDesc ={ sizeof( Matrix ) * header.boneCount, D3D11_USAGE_IMMUTABLE, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA bindBufferInitData ={ boneBindMatrices, 0, 0 };

	GameCheckFatalDx(  device->CreateBuffer( &bindBufferDesc, &bindBufferInitData, &this->bindMatricesBuffer ), "Couldn't create bind matrices buffer." );
}

void ModelBase::Read_Animations( const Header& header, unsigned char*& modelData )
{
	this->animCount = header.animCount;
	this->anims = newArray( Animation, header.animCount, AnimHeap::Instance(), ALIGN_4 );
	for( unsigned int i = 0; i < header.animCount; i++ )
	{
		// Animation constructor handles updating the modelData pointer
		this->anims[i] = Animation( header.boneCount, modelData );
	}
}

void ModelBase::Read_Vertices( ID3D11Device* device, const Header& header, unsigned char*& modelData )
{
	Vertex* vertices = reinterpret_cast<Vertex*>( modelData );
	modelData += header.vertexCount * sizeof( Vertex );

	D3D11_BUFFER_DESC vertexBufferDesc ={ sizeof( Vertex ) * header.vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA vertexBufferInitData ={ vertices, 0, 0 };

	GameCheckFatalDx(  device->CreateBuffer( &vertexBufferDesc, &vertexBufferInitData, &this->verticesBuffer ), "Couldn't create vertex buffer." );
}

void ModelBase::Read_Triangles( ID3D11Device* device, const Header& header, unsigned char*& modelData )
{
	Triangle* triangles = reinterpret_cast<Triangle*>( modelData );
	modelData += header.triangleCount * sizeof( Triangle );
	this->triangleVertexCount = header.triangleCount * 3;

	D3D11_BUFFER_DESC indexBufferDesc ={ sizeof( Triangle ) * header.triangleCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA indexBufferInitData ={ triangles, 0, 0 };

	GameCheckFatalDx(  device->CreateBuffer( &indexBufferDesc, &indexBufferInitData, &this->indicesBuffer ), "Couldn't create index buffer." );
}

void ModelBase::Read_Textures( ID3D11Device* device, const Header& header, unsigned char*& modelData, const char* const archiveFile )
{
	this->textureCount = header.textureCount;
	this->textureHead = nullptr;

	Texture* tail = nullptr;
	if( header.textureCount != 0 )
	{
		this->textureHead = TextureManager::Instance()->Add( device, archiveFile, reinterpret_cast<char*>( modelData ) );
		tail = this->textureHead;
		modelData += strlen( reinterpret_cast<char*>( modelData ) ) + 1;

		for( unsigned int i = 1; i < header.textureCount; i++ )
		{
			Texture* texture = TextureManager::Instance()->Add( device, archiveFile, reinterpret_cast<const char*>( modelData ) );
			tail->nextTexture = texture;
			tail = texture;
			modelData += strlen( reinterpret_cast<const char*>( modelData ) ) + 1;
		}
	}
}

void ModelBase::Set( ID3D11Device* device, const char* const archiveFile )
{
	unsigned char* modelName = nullptr;
	int modelNameSize;
	GameVerify( read_asset( archiveFile, MANIFEST_TYPE, "manifest", modelName, modelNameSize, TemporaryHeap::Instance() ) );

	unsigned char* modelData = nullptr;
	int modelSize;
	GameVerify( read_asset( archiveFile, VERTS_TYPE, reinterpret_cast<char*>( modelName ), modelData, modelSize, TemporaryHeap::Instance() ) );

	Header header = *reinterpret_cast<Header*>( modelData );
	modelData += sizeof( Header );

	// set bounding sphere from header (boundingRadius, boundsMatrix)
	this->boundingRadius = header.radius;
	Matrix scale( SCALE, header.radius, header.radius, header.radius );
	Matrix translate( TRANS, header.boundsCenterX, header.boundsCenterY, header.boundsCenterZ );
	this->boundsMatrix = scale * translate;

	// read hierarchy (boneCount, boneParentList, boneBindMatrices)
	Read_Hierarchy( device, header, modelData );
	
	// read animations (animCount, anims)
	Read_Animations( header, modelData );
	
	// read vertices (verticesBuffer)
	Read_Vertices( device, header, modelData );

	// read triangles (indicesBuffer, triangleVertexCount)
	Read_Triangles( device, header, modelData );
	
	// read textures (textureCount, textureHead)
	Read_Textures( device, header, modelData, archiveFile );
}

void ModelBase::Reset()
{
	ManagedObject::Reset();
	GameAssert( this->Get_Reference_Count() == 0 );

	while( this->instancesHead )
	{
		ModelBaseInstance* next = this->instancesHead->next;
		delete this->instancesHead;
		this->instancesHead = next;
	}

	this->verticesBuffer->Release();
	this->indicesBuffer->Release();
	
	delete this->boneParentList;
	this->bindMatricesBuffer->Release();

	delete this->anims;

	while( this->textureHead != nullptr )
	{
		Texture* head = this->textureHead;
		this->textureHead = head->nextTexture;
		TextureManager::Instance()->Remove( head );
	}
}

void ModelBase::Free_Me()
{
	ModelBaseManager::Instance()->Remove( this );
}

const Texture* ModelBase::Get_Texture( const uint32_t textureID ) const
{
	if( textureID >= this->textureCount )
	{
		return TextureManager::Instance()->Default_Texture();
	}
	else
	{
		Texture* curr = this->textureHead;
		for( unsigned int i = 0; i < textureID; i++ )
			curr = curr->nextTexture;

		return curr;
	}
}

const Matrix& ModelBase::Get_Bounding_Matrix() const
{
	return this->boundsMatrix;
}

float ModelBase::Get_Bounding_Radius() const
{
	return this->boundingRadius;
}

const static unsigned int VERTEX_SIZE = sizeof( Vertex );
const static unsigned int VERTEX_BUFFER_OFFSET = 0;

#define BIND_MATRICES_BUFFER_INDEX 2

void ModelBase::Draw( ID3D11DeviceContext* context, const Material* material, const Camera* camera, ID3DUserDefinedAnnotation* annotation ) const
{
	GameAssert( context );
	GameAssert( material );

	annotation->BeginEvent( L"ModelBase::Draw()" );
	{
		context->VSSetConstantBuffers( BIND_MATRICES_BUFFER_INDEX, 1, &bindMatricesBuffer );
		context->IASetVertexBuffers( 0, 1, &this->verticesBuffer, &VERTEX_SIZE, &VERTEX_BUFFER_OFFSET );
		context->IASetIndexBuffer( this->indicesBuffer, DXGI_FORMAT_R32_UINT, 0 );
		context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		for( ModelBaseInstance* model = this->instancesHead; model; model = model->next )
		{
			if( model->Should_Be_Drawn( material, camera ) )
			{
				model->Draw( context, this->triangleVertexCount, annotation );
			}
		}
	}
	annotation->EndEvent();
}

uint32_t ModelBase::Get_Bone_Count() const
{
	return this->boneCount;
}

Matrix ModelBase::Get_Bone_Transform( const uint32_t animID, const uint32_t boneIndex, const uint32_t time ) const
{
	GameAssert( animID < this->animCount );
	GameAssert( boneIndex < this->boneCount );

	return this->anims[animID].Get_Transform( time, boneIndex );
}

const Animation& ModelBase::Get_Animation( const uint32_t animID ) const
{
	GameAssert( animID < this->animCount );

	return this->anims[animID];
}

int ModelBase::Get_Parent_Bone_Of_Bone( unsigned int boneIndex ) const
{
	GameAssert( boneIndex < this->boneCount );

	return this->boneParentList[boneIndex];
}

Model* ModelBase::Create_Instance( ID3D11Device* device, const Material* material )
{
	Model* model = new( ModelHeap::Instance(), ALIGN_16 ) Model( device, material, this );
	model->next = this->instancesHead;
	if( this->instancesHead )
	{
		this->instancesHead->prev = model;
	}

	this->instancesHead = model;

	return model;
}

void ModelBase::Delete_Instance( ModelBaseInstance* instance )
{
	if( instance->prev )
	{
		instance->prev->next = instance->next;
	}
	else
	{
		GameAssert( instance == this->instancesHead );
		this->instancesHead = instance->next;
	}

	if( instance->next )
	{
		instance->next->prev = instance->prev;
	}

	delete instance;
}




ModelBaseInstance::ModelBaseInstance( ModelBase* inBaseModel)
	: next( nullptr ),
	prev( nullptr ),
	baseModel( inBaseModel )
{
	this->baseModel->Add_Reference();
}

ModelBaseInstance::~ModelBaseInstance()
{
	if( baseModel )
	{
		baseModel->Remove_Reference();
	}
}