#include <assert.h>

#include <File.h>
#include <MathEngine.h>
#include <malloc.h>
#include <new>
#include <eat.h>
#include <cstdio>
#include <PCSTree.h>

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

ModelBase::ModelBase() :
	ManagedObject(),
	ReferencedObject(),
	boneCount(0),
	boneMeshes(0),
	animCount(0),
	anims(0),
	boneParentList(0),
	textureCount(0),
	textureHead(0),
	boundsMatrix(),
	boundingRadius(0.0f)
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

	float u;
	float v;

	float normX;
	float normY;
	float normZ;
};



struct Triangle
{
	unsigned int vertexA;
	unsigned int vertexB;
	unsigned int vertexC;
};

const static Triangle pyramidTriList[] =
{
	{0,2,1}, // Good
	{1,2,3}, // Good
	
	{4,5,6}, // Good
	{7,6,5}, // Good

	{8,9,10}, // Good
	{9,11,10}, // Good

	{12,14,13}, // Good
	{13,14,15}, // Good
	
	{16,17,18}, // Good
	{17,19,18}, // Good

	{20,22,21}, // Good
	{21,22,23} // Good
};

const static Vertex cubeData[] =
{
	// left face
	{-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		-0.6f, -0.6f, -0.6f},
	{-0.5f, -0.5f,  0.5f,	0.0f, 1.0f,		-0.6f, -0.6f,  0.6f},
	{-0.5f,  0.5f, -0.5f,	1.0f, 0.0f,		-0.6f,  0.6f, -0.6f},
	{-0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		-0.6f,  0.6f,  0.6f},


	// right face
	{ 0.5f, -0.5f, -0.5f,   1.0f, 0.0f,		 0.6f, -0.6f, -0.6f},
	{ 0.5f, -0.5f,  0.5f,   0.0f, 1.0f,		 0.6f, -0.6f,  0.6f},
	{ 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		 0.6f,  0.6f, -0.6f},
	{ 0.5f,  0.5f,  0.5f,	0.0f, 0.0f,		 0.6f,  0.6f,  0.6f},

	// bottom face
	{-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		-0.6f, -0.6f, -0.6f},
	{-0.5f, -0.5f,  0.5f,	0.0f, 1.0f,		-0.6f, -0.6f,  0.6f},
	{ 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		 0.6f, -0.6f, -0.6f},
	{ 0.5f, -0.5f,  0.5f,	1.0f, 1.0f,		 0.6f, -0.6f,  0.6f},

	// top face
	{-0.5f,  0.5f, -0.5f,	0.0f, 0.0f,		-0.6f,  0.6f, -0.6f},
	{-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		-0.6f,  0.6f,  0.6f},
	{ 0.5f,  0.5f, -0.5f,	1.0f, 0.0f,		 0.6f,  0.6f, -0.6f},
	{ 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		 0.6f,  0.6f,  0.6f},

	// front face
	{-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		-0.6f, -0.6f,  0.6f},
	{-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		-0.6f,  0.6f,  0.6f},
	{ 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		 0.6f, -0.6f,  0.6f},
	{ 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		 0.6f,  0.6f,  0.6f},

	// back face
	{-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		-0.6f, -0.6f, -0.6f},
	{-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		-0.6f,  0.6f, -0.6f},
	{ 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		 0.6f, -0.6f, -0.6f},
	{ 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		 0.6f,  0.6f, -0.6f}
};


void ModelBase::Set(const char* const archiveFile)
{
	unsigned char* modelName;
	int modelNameSize;
	bool status = eat(archiveFile, MANIFEST_TYPE, "manifest", modelName, modelNameSize);
	assert(status);

	unsigned char* modelData;
	int modelSize;
	status = eat(archiveFile, VERTS_TYPE, reinterpret_cast<char*>(modelName), modelData, modelSize);
	assert(status);

	Header* header = reinterpret_cast<Header*>(modelData);
	this->boneParentList = reinterpret_cast<int*>(modelData + sizeof(Header));
	this->boneMeshes = new Mesh[header->boneCount];
	this->boneCount = header->boneCount;
	unsigned char* animData = reinterpret_cast<unsigned char*>(this->boneParentList) + header->boneCount * sizeof(int);

	this->animCount = header->animCount;
	this->anims = new Animation[this->animCount];
	for (unsigned int i = 0; i < header->animCount; i++)
	{
		this->anims[i] = std::move(Animation(header->boneCount, animData));
		animData += 4 + this->anims[i].Get_KeyFrame_Count() * (4 + header->boneCount * sizeof(Transform));
	}

	Vertex* vertices = reinterpret_cast<Vertex*>(animData);
	Triangle* triangles = reinterpret_cast<Triangle*>(reinterpret_cast<unsigned char*>(vertices) + header->vertexCount * sizeof(Vertex));

	this->boundingRadius = header->radius;
	Matrix scale(SCALE, header->radius, header->radius, header->radius);
	Matrix translate(TRANS, header->boundsCenterX, header->boundsCenterY, header->boundsCenterZ);
	this->boundsMatrix = scale * translate;

	this->textureCount = header->textureCount;
	this->textureHead = 0;

	char* ptr = reinterpret_cast<char*>(triangles) + header->triangleCount * sizeof(Triangle);
	Texture* tail = 0;
	if (header->textureCount != 0)
	{
		this->textureHead = TextureManager::Instance()->Add(archiveFile, ptr);
		tail = this->textureHead;
		ptr += strlen(ptr) + 1;

		for (unsigned int i = 1; i < header->textureCount; i++)
		{
			Texture* texture = TextureManager::Instance()->Add(archiveFile, ptr);
			tail->nextTexture = texture;
			tail = texture;
			ptr += strlen(ptr) + 1;
		}
	}
	
	// TODO remove this when ready to skin
	glGenVertexArrays(1, &this->boneMeshes[0].vao);
	glBindVertexArray(this->boneMeshes[0].vao);

	glGenBuffers(1, &this->boneMeshes[0].vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, this->boneMeshes[0].vboVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 24, cubeData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (3 * sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (5 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->boneMeshes[0].vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->boneMeshes[0].vboIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * 12, pyramidTriList, GL_STATIC_DRAW);
	glBindVertexArray(0);
	this->boneMeshes[0].triangleVertexCount = 36;

	for (unsigned int i = 1; i < header->boneCount; i++)
	{
		this->boneMeshes[i].triangleVertexCount = this->boneMeshes[0].triangleVertexCount;
		this->boneMeshes[i].vao = this->boneMeshes[0].vao;
		this->boneMeshes[i].vboIndices = this->boneMeshes[0].vboIndices;
		this->boneMeshes[i].vboVertices = this->boneMeshes[0].vboVertices;
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
	assert(this->Get_Reference_Count() == 0);

	// TODO remove this and replace it with the commented stuff
	glDeleteBuffers(1, &this->boneMeshes[0].vboIndices);
	glDeleteBuffers(1, &this->boneMeshes[0].vboVertices);
	glDeleteVertexArrays(1, &this->boneMeshes[0].vao);

	this->boneMeshes[0].vboIndices = 0;
	this->boneMeshes[0].vboVertices = 0;
	this->boneMeshes[0].vao = 0;

	//for (int i = 0; i < this->boneCount; i++)
	//{
	//	glDeleteBuffers(1, &this->boneMeshes[i].vboIndices);
	//	glDeleteBuffers(1, &this->boneMeshes[i].vboVertices);
	//	glDeleteVertexArrays(1, &this->boneMeshes[i].vao);

	//	this->boneMeshes[i].vboIndices = 0;
	//	this->boneMeshes[i].vboVertices = 0;
	//	this->boneMeshes[i].vao = 0;
	//}

	while (this->textureHead != 0)
	{
		Texture* head = this->textureHead;
		this->textureHead = head->nextTexture;
		TextureManager::Instance()->Remove(head);
	}

	
}

void ModelBase::Free_Me()
{
	ModelBaseManager::Instance()->Remove(this);
}

const Texture* ModelBase::Get_Texture(const uint32_t textureID) const
{
	if (textureID >= this->textureCount)
		return TextureManager::Instance()->Default_Texture();
	else
	{
		Texture* curr = this->textureHead;
		for (unsigned int i = 0; i < textureID; i++)
			curr = curr->nextTexture;

		return curr;
	}
}

const Matrix& ModelBase::Get_Bounding_Matrix() const
{
	return this->boundsMatrix;
}

const float ModelBase::Get_Bounding_Radius() const
{
	return this->boundingRadius;
}

void ModelBase::Draw(const uint32_t boneIndex) const
{
	assert(boneIndex < this->boneCount);

	glBindVertexArray(this->boneMeshes[boneIndex].vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->boneMeshes[boneIndex].vboVertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->boneMeshes[boneIndex].vboIndices);
	glDrawElements(GL_TRIANGLES, this->boneMeshes[boneIndex].triangleVertexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

Bone* const ModelBase::Create_Skeleton_From_Model() const
{
	Bone** boneyard = new Bone*[this->boneCount];
	for (unsigned int i = 0; i < this->boneCount; i++)
		boneyard[i] = BoneManager::Instance()->Add(i);

	Bone* root = 0;
	for (unsigned int i = 0; i < this->boneCount; i++)
	{
		int parent = this->boneParentList[i];
		if (parent == -1)
		{
			if (root == 0)
			{
				root = boneyard[i];
			}
			else
			{
				boneyard[i]->setSibling(root);
				root = boneyard[i];
			}
		}
		else
		{
			boneyard[i]->setSibling(boneyard[parent]->getChild());
			boneyard[parent]->setChild(boneyard[i]);
			boneyard[i]->setParent(boneyard[parent]);
		}
	}

	return root;
}

const Matrix ModelBase::Get_Bone_Transform(const uint32_t animID, const uint32_t boneIndex, const uint32_t time) const
{
	assert(animID < this->animCount);
	assert(boneIndex < this->boneCount);

	return this->anims[animID].Get_Transform(time, boneIndex);
}

const Animation& ModelBase::Get_Animation(const uint32_t animID) const
{
	assert(animID < this->animCount);

	return this->anims[animID];
}