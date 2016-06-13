#include <GameAssert.h>

#include <File.h>
#include <MathEngine.h>
#include <malloc.h>
#include <new>
#include <asset_reader.h>
#include <stdio.h>
#include <utility>
#include <d3d11_1.h>
#include <thread>
#include <atomic>
#include <Mutex.h>

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
#include "SharedShaderDefines.h"

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
	boneHierarchyBuffer( nullptr ),
	animCount( 0 ),
	anims( nullptr ),
	textureCount( 0 ),
	textureHead( nullptr ),
	instancesHead( nullptr ),
	modelBaseLock( new( MiscHeap::Instance(), ALIGN_4 ) Mutex() )
{
	this->modelBaseLock->LockMutex( std::memory_order_seq_cst );
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
	GameCheckFatal( header.boneCount <= MAX_BONES, "Too many bones in model." );
	Matrix* boneBindMatrices = newArray( Matrix, MAX_BONES, AnimHeap::Instance(), ALIGN_16 );
	// We need to account for padding, that is why the boneParentList is much bigger than you might think it should be
	int* boneParentList = newArray( int, MAX_BONES * 4, AnimHeap::Instance(), ALIGN_4 );

	for( unsigned int i = 0; i < header.boneCount; i++ )
	{
		unsigned int baseIndex = i * 4;

		// Set all four values, just to be safe
		boneParentList[baseIndex] = *reinterpret_cast<int*>( modelData );
		boneParentList[baseIndex + 1] = *reinterpret_cast<int*>( modelData );
		boneParentList[baseIndex + 2] = *reinterpret_cast<int*>( modelData );
		boneParentList[baseIndex + 3] = *reinterpret_cast<int*>( modelData );

		modelData += sizeof( int );

		boneBindMatrices[i] = *reinterpret_cast<Matrix*>( modelData );
		modelData += sizeof( Matrix );
	}

	for( unsigned int i = header.boneCount; i < MAX_BONES;  i++ )
	{
		unsigned int baseIndex = i * 4;
		boneParentList[baseIndex] = -1;
		boneParentList[baseIndex + 1] = -1;
		boneParentList[baseIndex + 2] = -1;
		boneParentList[baseIndex + 3] = -1;
		boneBindMatrices[i] = Matrix();
	}

	D3D11_BUFFER_DESC hierarchyDesc;
	hierarchyDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hierarchyDesc.ByteWidth = sizeof( INT ) * MAX_BONES * 4;
	hierarchyDesc.CPUAccessFlags = 0;
	hierarchyDesc.MiscFlags = 0;
	hierarchyDesc.StructureByteStride = 0;
	hierarchyDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA hierarchyInitDesc;
	hierarchyInitDesc.pSysMem = boneParentList;
	hierarchyInitDesc.SysMemPitch = 0;
	hierarchyInitDesc.SysMemSlicePitch = 0;

	GameCheckFatalDx( device->CreateBuffer( &hierarchyDesc, &hierarchyInitDesc, &this->boneHierarchyBuffer ), "Couldn't create bone hierarchy buffer." );

	delete boneParentList;

	D3D11_BUFFER_DESC bindPoseDesc ={ sizeof( Matrix ) * MAX_BONES, D3D11_USAGE_IMMUTABLE, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA bindPoseInitDesc ={ boneBindMatrices, 0, 0 };

	GameCheckFatalDx( device->CreateBuffer( &bindPoseDesc, &bindPoseInitDesc, &this->bindPoseBuffer ), "Couldn't create bind bose buffer." );

	delete boneBindMatrices;
}

void ModelBase::Read_Animations( ID3D11Device* device, const Header& header, unsigned char*& modelData )
{
	this->animCount = header.animCount;
	this->anims = newArray( Animation, header.animCount, AnimHeap::Instance(), ALIGN_4 );
	memset( this->anims, 0, sizeof( Animation ) * header.animCount );
	for( unsigned int i = 0; i < header.animCount; i++ )
	{
		// Animation constructor handles updating the modelData pointer
		this->anims[i] = Animation( device, header.boneCount, modelData );
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
	static unsigned int NEXT_GENERATED_ID_NUMBER = 0;

	unsigned char* modelName = nullptr;
	int modelNameSize;
	GameVerify( read_asset( archiveFile, MANIFEST_TYPE, "manifest", modelName, modelNameSize, TemporaryHeap::Instance() ) );

	if( strlen( reinterpret_cast<char*>( modelName ) ) == 0 )
	{
		unsigned int idNum = NEXT_GENERATED_ID_NUMBER++;
		char idStr[16];
		sprintf_s( idStr, "ModelBase %u", idNum );
		this->SetID( idStr );
	}
	else
	{
		this->SetID( reinterpret_cast<char*>( modelName ) );
	}

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
	Read_Animations( device, header, modelData );
	
	// read vertices (verticesBuffer)
	Read_Vertices( device, header, modelData );

	// read triangles (indicesBuffer, triangleVertexCount)
	Read_Triangles( device, header, modelData );
	
	// read textures (textureCount, textureHead)
	Read_Textures( device, header, modelData, archiveFile );

	this->modelBaseLock->UnlockMutex();
}

void ModelBase::Reset()
{
	this->modelBaseLock->LockMutex( std::memory_order_seq_cst );

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
	
	this->boneHierarchyBuffer->Release();
	this->bindPoseBuffer->Release();

	// We have to manually call deconstructors, as my array allocator won't do it by itself
	// However, the first anim's destructor will be called automatically
	for( unsigned int i = 1; i < this->animCount; i++ )
	{
		this->anims[i].~Animation();
	}
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
	Mutex::Lock lock( *this->modelBaseLock, std::memory_order_acquire );

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

void ModelBase::Update_Model_Skeletons( ID3D11DeviceContext* context, ID3DUserDefinedAnnotation* annotation ) const
{
	GameAssert( context );
	GameAssert( annotation );

	annotation->BeginEvent( L"ModelBase::Update_Model_Skeletons()" );
	{
		if( strcmp( this->ID, "ModelBase 1" ) == 0 )
		{
			int a = 3;
			a++;
		}
		Mutex::Lock lock( *this->modelBaseLock, std::memory_order_acquire );

		context->CSSetConstantBuffers( CS_HIERARCHY_REGISTER, 1, &this->boneHierarchyBuffer );
		context->CSSetConstantBuffers( CS_BIND_POSE_REGISTER, 1, &this->bindPoseBuffer );
		for( ModelBaseInstance* model = this->instancesHead; model; model = model->next )
		{
			model->Update_Skeleton( context, annotation );
		}
	}
	annotation->EndEvent();
}

const static unsigned int VERTEX_SIZE = sizeof( Vertex );
const static unsigned int VERTEX_BUFFER_OFFSET = 0;

void ModelBase::Draw( ID3D11DeviceContext* context, const Material* material, const Camera* camera, ID3DUserDefinedAnnotation* annotation ) const
{
	GameAssert( context );
	GameAssert( material );
	GameAssert( annotation );

	annotation->BeginEvent( L"ModelBase::Draw()" );
	{
		Mutex::Lock lock( *this->modelBaseLock, std::memory_order_acquire );

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

const Animation& ModelBase::Get_Animation( const uint32_t animID ) const
{
	Mutex::Lock lock( *this->modelBaseLock, std::memory_order_acquire );

	GameAssert( animID < this->animCount );

	return this->anims[animID];
}

Model* ModelBase::Create_Instance( ID3D11Device* device, const Material* material )
{
	Mutex::Lock lock( *this->modelBaseLock, std::memory_order_acquire );

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
	Mutex::Lock lock( *this->modelBaseLock, std::memory_order_acquire );

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