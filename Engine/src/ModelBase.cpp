#include <GameAssert.h>

#include <File.h>
#include <MathEngine.h>
#include <malloc.h>
#include <new>
#include <asset_reader.h>
#include <stdio.h>
#include <utility>
#include <d3d11_1.h>

#include "MemorySetup.h"
#include "DrawInfo.h"
#include "ModelBase.h"
#include "ModelBaseManager.h"
#include "Texture.h"
#include "TextureManager.h"
#include "GlobalHeaps.h"
#include "Mesh.h"
#include "Animation.h"
#include "BoneManager.h"
#include "Bone.h"
#include "KeyFrame.h"

ModelBase::ModelBase()
	: ManagedObject(),
	ReferencedObject(),
	boneCount( 0 ),
	boneMeshes( nullptr ),
	animCount( 0 ),
	anims( nullptr ),
	boneParentList( nullptr ),
	textureCount( 0 ),
	textureHead( nullptr ),
	boundsMatrix(),
	boundingRadius( 0.0f )
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


void ModelBase::Set( ID3D11Device* device, const char* const archiveFile )
{
	unsigned char* modelName = nullptr;
	int modelNameSize;
	GameVerify( read_asset( archiveFile, MANIFEST_TYPE, "manifest", modelName, modelNameSize, TemporaryHeap::Instance() ) );

	unsigned char* modelData = nullptr;
	int modelSize;
	GameVerify( read_asset( archiveFile, VERTS_TYPE, reinterpret_cast<char*>( modelName ), modelData, modelSize, TemporaryHeap::Instance() ) );

	Header* header = reinterpret_cast<Header*>( modelData );
	this->boneParentList = newArray( int, header->boneCount, AnimHeap::Instance(), ALIGN_4 );
	memcpy( this->boneParentList, reinterpret_cast<int*>( modelData + sizeof( Header ) ), sizeof( int ) * header->boneCount );

	this->boneMeshes = newArray( Mesh, header->boneCount, AnimHeap::Instance(), ALIGN_4 );
	this->boneCount = header->boneCount;
	unsigned char* animData = modelData + sizeof( Header ) + header->boneCount * sizeof( int );

	this->animCount = header->animCount;
	this->anims = newArray( Animation, this->animCount, AnimHeap::Instance(), ALIGN_4 );
	for( unsigned int i = 0; i < header->animCount; i++ )
	{
		this->anims[i] = Animation( header->boneCount, animData );
		animData += 4 + this->anims[i].Get_KeyFrame_Count() * ( 4 + header->boneCount * sizeof( Transform ) );
	}

	Vertex* vertices = reinterpret_cast<Vertex*>( animData );
	Triangle* triangles = reinterpret_cast<Triangle*>( reinterpret_cast<unsigned char*>(vertices) +header->vertexCount * sizeof( Vertex ) );

	this->boundingRadius = header->radius;
	Matrix scale( SCALE, header->radius, header->radius, header->radius );
	Matrix translate( TRANS, header->boundsCenterX, header->boundsCenterY, header->boundsCenterZ );
	this->boundsMatrix = scale * translate;

	this->textureCount = header->textureCount;
	this->textureHead = 0;

	char* ptr = reinterpret_cast<char*>(triangles) + header->triangleCount * sizeof( Triangle );
	Texture* tail = 0;
	if( header->textureCount != 0 )
	{
		this->textureHead = TextureManager::Instance()->Add( device, archiveFile, ptr );
		tail = this->textureHead;
		ptr += strlen( ptr ) + 1;

		for( unsigned int i = 1; i < header->textureCount; i++ )
		{
			Texture* texture = TextureManager::Instance()->Add( device, archiveFile, ptr );
			tail->nextTexture = texture;
			tail = texture;
			ptr += strlen( ptr ) + 1;
		}
	}

	// TODO remove this when ready to skin
	D3D11_BUFFER_DESC vertexBufferDesc ={ sizeof( Vertex ) * 24, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA vertexBufferInitData ={ cubeData, 0, 0 };

	GameCheckFatal( SUCCEEDED( device->CreateBuffer( &vertexBufferDesc, &vertexBufferInitData, &this->boneMeshes[0].verticesBuffer ) ), "Couldn't create vertex buffer." );

	D3D11_BUFFER_DESC indexBufferDesc ={ sizeof( Triangle ) * 12, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA indexBufferInitData ={ cubeTriList, 0, 0 };

	GameCheckFatal( SUCCEEDED( device->CreateBuffer( &indexBufferDesc, &indexBufferInitData, &this->boneMeshes[0].indicesBuffer ) ), "Couldn't create index buffer." );

	this->boneMeshes[0].triangleVertexCount = 36;

	for( unsigned int i = 1; i < this->boneCount; i++ )
	{
		this->boneMeshes[i].triangleVertexCount = this->boneMeshes[0].triangleVertexCount;
		this->boneMeshes[i].verticesBuffer = this->boneMeshes[0].verticesBuffer;
		this->boneMeshes[i].indicesBuffer = this->boneMeshes[0].indicesBuffer;
	}
	// TODO stop removing here

	//this->triangleVertexCount = header->triangleCount * 3;

	//glGenVertexArrays(1, &this->vao);
	//glBindVertexArray(this->vao);

	//glGenBuffers(1, &this->vboVertices);
	//glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * header->vertexCount, vertices, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (3 * sizeof(float)));
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (5 * sizeof(float)));

	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);

	//glGenBuffers(1, &this->vboIndices);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * header->triangleCount, triangles, GL_STATIC_DRAW);
	//

	//// Disable model's vao
	//glBindVertexArray(0);
}

void ModelBase::Reset()
{
	ManagedObject::Reset();
	GameAssert( this->Get_Reference_Count() == 0 );

	// TODO change this to release buffers in every bone mesh when skinning
	this->boneMeshes[0].indicesBuffer->Release();
	this->boneMeshes[0].verticesBuffer->Release();

	delete this->boneParentList;
	delete this->anims;
	delete this->boneMeshes;

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
		return TextureManager::Instance()->Default_Texture();
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

void ModelBase::Draw( const DrawInfo& info ) const
{
	GameAssert( info.boneIndex < this->boneCount );


	info.context->IASetVertexBuffers( 0, 1, &this->boneMeshes[info.boneIndex].verticesBuffer, &VERTEX_SIZE, &VERTEX_BUFFER_OFFSET );
	info.context->IASetIndexBuffer( this->boneMeshes[info.boneIndex].indicesBuffer, DXGI_FORMAT_R32_UINT, 0 );
	info.context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	info.context->DrawIndexed( this->boneMeshes[info.boneIndex].triangleVertexCount, 0, 0 );
}

Bone* const ModelBase::Create_Skeleton_From_Model() const
{
	Bone** boneyard = new( AnimHeap::Instance(), ALIGN_4 )  Bone*[this->boneCount];
	for( unsigned int i = 0; i < this->boneCount; i++ )
		boneyard[i] = BoneManager::Instance()->Add( i );

	Bone* root = 0;
	for( unsigned int i = 0; i < this->boneCount; i++ )
	{
		int parent = this->boneParentList[i];
		if( parent == -1 )
		{
			if( root == 0 )
			{
				root = boneyard[i];
			}
			else
			{
				boneyard[i]->setSibling( root );
				root = boneyard[i];
			}
		}
		else
		{
			boneyard[i]->setSibling( boneyard[parent]->getChild() );
			boneyard[parent]->setChild( boneyard[i] );
			boneyard[i]->setParent( boneyard[parent] );
		}
	}

	return root;
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