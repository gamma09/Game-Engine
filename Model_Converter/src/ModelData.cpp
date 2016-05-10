#include "ModelData.h"

#include <MathEngine.h>
#include <climits>

#include "DebugPrint.h"


ModelData::ModelData() :
	vertexList(),
	triangleList(),
	textureList(),
	filename()
{
	// Do nothing
}

ModelData::ModelData( const ModelData& data ) :
	vertexList( data.vertexList ),
	triangleList( data.triangleList ),
	textureList( data.textureList )
{
	this->filename = new char[strlen( data.filename ) + 1];
	strcpy_s( this->filename, strlen( data.filename ) + 1, data.filename );

	strcpy_s( this->title, data.title );
}

ModelData& ModelData::operator=( const ModelData& data )
{
	this->vertexList = data.vertexList;
	this->triangleList = data.triangleList;
	this->textureList = data.textureList;

	strcpy_s( this->title, data.title );

	this->filename = new char[strlen( data.filename ) + 1];
	strcpy_s( this->filename, strlen( data.filename ) + 1, data.filename );

	return *this;
}

ModelData::~ModelData()
{
	delete[] this->filename;
}

unsigned int ModelData::Add_Vertex( const Vertex& v )
{
	unsigned int i = 0;
	for( ; i < this->vertexList.size(); i++ )
	{
		if( this->vertexList[i].Equals( v ) )
			return i;
	}

	this->vertexList.push_back( v );
	return i;
}

const float TripleProduct( const Vertex& v0, const Vertex& v1, const Vertex& v2 )
{
	Vect vect0( v0.x, v0.y, v0.z );
	Vect vect1( v1.x, v1.y, v1.z );
	Vect vect2( v2.x, v2.y, v2.z );

	Vect a = vect2 - vect0;
	Vect b = vect1 - vect0;

	Vect normal( v0.nx, v0.ny, v0.nz );

	return a.cross( b ).dot( normal );
}

void ModelData::Add_Triangle( const Vertex& v0, const Vertex& v1, const Vertex& v2 )
{
	unsigned int index0;
	unsigned int index1;
	unsigned int index2;

	if( TripleProduct( v0, v1, v2 ) > 0.0f )
	{
		index0 = this->Add_Vertex( v0 );
		index1 = this->Add_Vertex( v1 );
		index2 = this->Add_Vertex( v2 );
	}
	else
	{
		index0 = this->Add_Vertex( v0 );
		index1 = this->Add_Vertex( v2 );
		index2 = this->Add_Vertex( v1 );
	}

	Triangle tri( index0, index1, index2 );

	this->triangleList.push_back( tri );
	this->triangleTextureList.push_back( -1 );
}


void ModelData::Add_Texture( const char* textureName, const char* textureFile )
{
	Texture texture( textureName, textureFile );

	for( auto it = this->textureList.begin(); it != this->textureList.end(); it++ )
	{
		if( *it == texture )
			return;
	}

	this->textureList.push_back( texture );
}

int ModelData::Add_Bone( const Bone& bone )
{
	int index = this->bones.size();
	this->bones.push_back( bone );
	return index;
}

void ModelData::Add_Animation( const Animation& anim )
{
	this->anims.push_back( anim );
}

void ModelData::Add_Influence( int controlPointIndex, const BoneInfluence& influence )
{
	for( unsigned int i = 0; i < this->vertexList.size(); i++ )
	{
		if( this->vertexList[i].ControlPointIndex == controlPointIndex )
		{
			this->vertexList[i].Add_Bone_Influence( influence );
		}
	}
}

void ModelData::Bind_Bone( unsigned int boneIndex, const Matrix& invBindMatrix )
{
	this->bones[boneIndex].invBindMatrix = invBindMatrix;
}

void ModelData::Set_Triangle_Texture( const unsigned int triangle, const short texture )
{
	this->triangleTextureList[triangle] = texture;
}

void ModelData::Set_Base_Filename( const char* fbxFilename )
{
	this->filename = new char[strlen( fbxFilename ) - 4 + 1];
	int length = strlen( fbxFilename ) - 4;

	for( int i = 0; i < length; i++ )
		this->filename[i] = fbxFilename[i];

	this->filename[length] = 0;
}

void ModelData::Set_Title( const char* _title )
{
	this->title[TITLE_SIZE - 1] = 0;
	strncpy_s( this->title, _title, TITLE_SIZE - 1 );

	// Convert title to lower case (for when the engine is loading it in)
	for( int i = strlen( this->title ) - 1; i >= 0; i-- )
	{
		if( this->title[i] >= 'A' && this->title[i] <= 'Z' )
			this->title[i] += 'a' - 'A';
	}
}

const char* ModelData::Get_Base_Filename() const
{
	return &*this->filename;
}

const char* ModelData::Get_Title() const
{
	return &*this->title;
}

const vector<Vertex>& ModelData::Get_Vertex_List() const
{
	return this->vertexList;
}

const vector<Triangle>& ModelData::Get_Triangle_List() const
{
	return this->triangleList;
}

const vector<Texture>& ModelData::Get_Texture_List() const
{
	return this->textureList;
}

const vector<short>& ModelData::Get_Triangle_Texture_List() const
{
	return this->triangleTextureList;
}

const vector<Bone>& ModelData::Get_Bones() const
{
	return this->bones;
}

const vector<Animation> ModelData::Get_Animations() const
{
	return this->anims;
}

void ModelData::Get_Bounding_Sphere( float& radius, float& centerX, float& centerY, float& centerZ ) const
{
	Vertex v0 = this->vertexList[0];
	Vertex v1 = this->vertexList[0];

	float maxDistSqr = 0.0f;

	// I know we can do faster in terms of asymptotic bounds, but faster = harder to read, and this is all done offline anyway, so
	// speed doesn't matter...
	for( unsigned int i = 0; i < this->vertexList.size(); i++ )
	{
		for( unsigned int j = i + 1; j < this->vertexList.size(); j++ )
		{
			const Vertex& a = this->vertexList[i];
			const Vertex& b = this->vertexList[j];
			float dx = a.x - b.x;
			float dy = a.y - b.y;
			float dz = a.z - b.z;

			float distSqr = dx * dx + dy * dy + dz * dz;
			if( distSqr > maxDistSqr )
			{
				v0 = a;
				v1 = b;
				maxDistSqr = distSqr;
			}
		}
	}

	centerX = ( v0.x + v1.x ) / 2.0f;
	centerY = ( v0.y + v1.y ) / 2.0f;
	centerZ = ( v0.z + v1.z ) / 2.0f;

	float radiusSqr = maxDistSqr / 4.0f;

	for( unsigned int i = 0; i < this->vertexList.size(); i++ )
	{
		const Vertex& v = this->vertexList[i];
		float dx = v.x - centerX;
		float dy = v.y - centerY;
		float dz = v.z - centerZ;

		float distSqr = dx * dx + dy * dy + dz * dz;
		if( distSqr > radiusSqr )
		{
			float oldRadius = sqrtf( radiusSqr );
			float delta = sqrtf( distSqr ) - oldRadius;
			Vect vec( v.x - centerX, v.y - centerY, v.z - centerZ );
			vec.norm();
			vec *= delta / 2.0f;
			centerX += vec[X];
			centerY += vec[Y];
			centerZ += vec[Z];
			radiusSqr = oldRadius + delta / 2.0f;
			radiusSqr *= radiusSqr;
		}
	}

	radius = sqrtf( radiusSqr );
}

void ModelData::Normalize_Influences()
{
	for( Vertex v : this->vertexList )
	{
		v.Normalize_Bone_Influences();
	}
}
