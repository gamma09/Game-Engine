#include <Logger.h>

#include "FBXImporter.h"
#include "fbxsdk.h"
#include "FBXInit.h"
#include "Triangle.h"

static FbxNode* const FindNode( const FbxNodeAttribute::EType criteria, FbxNode* const root )
{


	assert( nullptr != root );

	FbxNode * result = nullptr;
	const FbxNodeAttribute* const attribute = root->GetNodeAttribute();
	if( NULL != attribute && criteria == attribute->GetAttributeType() )
	{
		result = root;
	}
	else
	{
		for( int i = 0; i < root->GetChildCount() && nullptr == result; i++ )
		{
			result = FindNode( criteria, root->GetChild( i ) );
		}
	}

	return result;
}



FBXImporter::FBXImporter()
{
	InitializeSdkObjects( manager, scene );
}

FBXImporter::~FBXImporter()
{
	this->scene->Destroy();
	this->manager->Destroy();
}

void FBXImporter::Add_Extra_Texture( const char* additionalTexture )
{
	this->modelData.Add_Texture( additionalTexture, additionalTexture );
}

void FBXImporter::Import( const string& path, const string& filename )
{
	this->modelData.Set_Base_Filename( filename.c_str() );

	LoadScene( manager, scene, path, filename );

	FbxGeometryConverter geometryConverter( this->manager );
	geometryConverter.Triangulate( this->scene, true );

	this->ImportMesh();
	this->ImportSkeleton();
	this->ImportAnimations();
	this->ImportSkin();
}

void FBXImporter::ImportMesh()
{
	Logger::Log_Info( "Importing Mesh" );

	GameAssert( this->scene->GetRootNode() );
	FbxNode* node = FindNode( FbxNodeAttribute::eMesh, this->scene->GetRootNode() );
	GameAssert( node );
	FbxMesh* mesh = static_cast<FbxMesh*>( node->GetNodeAttribute() );
	GameAssert( mesh );

	Logger::Log_Debug( "  Mesh Name: %s", mesh->GetName() );
	this->modelData.Set_Title( mesh->GetName() );

	mesh->GenerateNormals( true, false, false );

	int polygonCount = mesh->GetPolygonCount();

	for( int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++ )
	{
		this->ImportTriangle( mesh, polygonIndex );
	}

	this->ImportTextures( node, mesh );
}

void FBXImporter::ImportTextures( FbxNode* node, FbxMesh* mesh )
{
	int materialCount = node->GetMaterialCount();

	for( int materialIndex = 0; materialIndex < materialCount; materialIndex++ )
	{
		FbxSurfaceMaterial* material = node->GetMaterial( materialIndex );

		Logger::Log_Debug( "    Material %d", materialIndex );
		Logger::Log_Debug( "      Name: \"%s\"", material->GetName() );
		Logger::Log_Debug( "      Textures:" );

		for( int textureLayerIndex = 0; textureLayerIndex < FbxLayerElement::sTypeTextureCount; textureLayerIndex++ )
		{
			FbxProperty prop = material->FindProperty( FbxLayerElement::sTextureChannelNames[textureLayerIndex] );
			GameAssert( prop.IsValid() );

			int textureCount = prop.GetSrcObjectCount<FbxTexture>();
			GameCheckFatal( prop.GetSrcObjectCount<FbxLayeredTexture>() == 0, "Error: Layered textures are not supported." );

			for( int textureIndex = 0; textureIndex < textureCount; textureIndex++ )
			{
				Logger::Log_Debug( "        Texture %d", textureIndex );
				FbxFileTexture* texture = FbxCast<FbxFileTexture>( prop.GetSrcObject<FbxTexture>( textureIndex ) );
				GameCheckFatal( texture, "Error: Only file-based textures are supported." );

				char name[20];
				memset( name, 0, 20 );
				const char* nameStr = texture->GetName();
				for( int i = min( 18, strlen( nameStr ) ); i >= 0; i-- )
				{
					name[i] = nameStr[i];
				}

				const char* filename = texture->GetFileName();
				const char* ptr = strrchr( filename, '\\' );
				if( ptr )
				{
					filename = ptr + 1;
				}

				Logger::Log_Debug( "          Name: \"%s\"", name );
				Logger::Log_Debug( "          File: \"%s\"", filename );

				modelData.Add_Texture( name, filename );
			}
		}
	}

	Logger::Log_Debug( "    Polygon to Material Mappings" );

	int polygonCount = mesh->GetPolygonCount();

	// so far as I know, there can only be one polygon-to-material mapping?
	// Or perhaps the number of material elements equals the number of texture channels?
	// Or perhaps the number of material elements equals the number of different ways to skin (texture) the cat?
	// who knows...
	int count = mesh->GetElementMaterialCount();
	GameAssert( count <= 1 );
	for( int materialIndex = 0; materialIndex < count; materialIndex++ )
	{
		FbxGeometryElementMaterial* leMat = mesh->GetElementMaterial( materialIndex );
		GameAssert( leMat );

		FbxLayerElement::EMappingMode mapMode = leMat->GetMappingMode();
		FbxLayerElement::EReferenceMode refMode = leMat->GetReferenceMode();

		Logger::Log_Debug( "      Mapping %d:", materialIndex );

		GameCheckFatal( refMode == FbxGeometryElement::eIndex || refMode == FbxGeometryElement::eIndexToDirect, "refMode must be either eIndex or eIndexToDirect" );
		GameCheckFatal( mapMode == FbxGeometryElement::eAllSame || mapMode == FbxGeometryElement::eByPolygon, "mapMode must be either eAllSame or eByPolygon" );
		if( mapMode == FbxGeometryElement::eAllSame )
		{
			GameCheckFatal( leMat->GetIndexArray().GetCount(), "Error: no material is specified in the polygon-to-material mapping" );

			int materialID = leMat->GetIndexArray().GetAt( 0 );
			for( int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++ )
			{
				Logger::Log_Debug( "        Polygon %5d uses Material %d", polygonIndex, materialID );
				modelData.Set_Triangle_Texture( polygonIndex, (short) materialID );
			}
		}
		else // FbxGeometryElement::eByPolygon
		{
			auto mapping = leMat->GetIndexArray();
			GameCheckFatal( mapping.GetCount() == polygonCount, "Error: some polygons don't have associated materials!" );

			for( int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++ )
			{
				Logger::Log_Debug( "        Polygon %5d uses Material %d", polygonIndex, mapping.GetAt( polygonIndex ) );
				modelData.Set_Triangle_Texture( polygonIndex, (short) mapping.GetAt( polygonIndex ) );
			}
		}
	}
}

void FBXImporter::ImportTriangle( FbxMesh* mesh, int polygonIndex )
{
	GameAssert( mesh->GetPolygonSize( polygonIndex ) == 3 );

	Logger::Log_Debug( "  Triangle %d", polygonIndex );
	Vertex v0 = this->ImportVertex( mesh, polygonIndex, 0 );
	Vertex v1 = this->ImportVertex( mesh, polygonIndex, 1 );
	Vertex v2 = this->ImportVertex( mesh, polygonIndex, 2 );

	modelData.Add_Triangle( v0, v1, v2 );
}

Vertex FBXImporter::ImportVertex( FbxMesh* mesh, int polygonIndex, int vertexIndex )
{
	FbxVector4* controlPoints = mesh->GetControlPoints();
	
	Vertex v;
	v.ControlPointIndex = mesh->GetPolygonVertex( polygonIndex, vertexIndex );
	v.x = (float) controlPoints[v.ControlPointIndex][0];
	v.y = (float) controlPoints[v.ControlPointIndex][1];
	v.z = (float) controlPoints[v.ControlPointIndex][2];
	v.w = 1.0f;

	Logger::Log_Debug( "    Control Point: %d", v.ControlPointIndex );
	Logger::Log_Debug( "    Coordinates: (%4.2f, %4.2f, %4.2f, %4.2f)", v.x, v.y, v.z, v.w );

	this->ImportVertexUV( mesh, polygonIndex, vertexIndex, v );
	this->ImportVertexNormal( mesh, polygonIndex, vertexIndex, v );

	return v;
}

void FBXImporter::ImportVertexUV( FbxMesh* mesh, int polygonIndex, int vertexIndex, Vertex& vtx ) const
{
	GameCheckFatal( mesh->GetElementUVCount() == 1, "This converter only supports meshes with exactly one UV element!" );
	FbxGeometryElementUV* leUV = mesh->GetElementUV( 0 );
	int cpOrUVIndex;
	if( leUV->GetMappingMode() == FbxGeometryElement::eByControlPoint )
	{
		if( leUV->GetReferenceMode() == FbxGeometryElement::eDirect )
		{
			cpOrUVIndex = vtx.ControlPointIndex;
		}
		else // eIndexToDirect
		{
			cpOrUVIndex = leUV->GetIndexArray().GetAt( vtx.ControlPointIndex );
		}
	}
	else // eByPolygonVertex
	{
		cpOrUVIndex = mesh->GetTextureUVIndex( polygonIndex, vertexIndex );
	}

	FbxVector2 uv = leUV->GetDirectArray().GetAt( cpOrUVIndex );
	vtx.u = (float) ( (double*) uv )[0];
	vtx.v = (float) ( (double*) uv )[1];

	static int VALUE = 0;
	VALUE++;

	Logger::Log_Debug( "    UV: (%1.2f, %1.2f)", vtx.u, vtx.v );
}

void FBXImporter::ImportVertexNormal( FbxMesh* mesh, int polygonIndex, int vertexIndex, Vertex& vtx ) const
{
	GameCheckFatal( mesh->GetElementNormalCount() == 1, "This converter only supports meshes with exactly one normal element!" );
	FbxGeometryElementNormal* leNormal = mesh->GetElementNormal( 0 );

	GameCheckFatal( leNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint || leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex, "This converter only supports normal elements that map by control point or polygon vertex!" );

	int directID;
	if( leNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint )
	{
		if( leNormal->GetReferenceMode() == FbxGeometryElement::eDirect )
		{
			directID = vtx.ControlPointIndex;
		}
		else // eIndexToDirect
		{
			directID = leNormal->GetIndexArray().GetAt( vtx.ControlPointIndex );
		}
	}
	else // eByPolygonVertex
	{
		directID = mesh->GetTextureUVIndex( polygonIndex, vertexIndex );
	}

	
	

	FbxVector4 normal = leNormal->GetDirectArray().GetAt( directID );
	vtx.nx = (float) normal[0];
	vtx.ny = (float) normal[1];
	vtx.nz = (float) normal[2];
	vtx.nw = 0.0f;


	Logger::Log_Debug( "    Normal: (%1.2f, %1.2f, %1.2f, %1.2f)", vtx.nx, vtx.ny, vtx.nz, vtx.nw );
}

static void ImportSkeletonRecursive( int prefixCount, const FbxNode* node, int parentIndex, ModelData& modelData )
{
	GameAssert( node );

	Bone bone;
	bone.name = node->GetName();
	bone.parentBone = parentIndex;

	Logger::Log_Debug( "%*sBone: %s", prefixCount * 2, "", bone.name.c_str() );

	int myIndex = modelData.Add_Bone( bone );

	for( int i = 0; i < node->GetChildCount(); i++ )
	{
		ImportSkeletonRecursive( prefixCount + 1, node->GetChild( i ), myIndex, modelData );
	}
}

void FBXImporter::ImportSkeleton()
{
	Logger::Log_Info( "Importing Skeleton..." );

	const FbxNode* skeletonRoot = FindNode( FbxNodeAttribute::eSkeleton, this->scene->GetRootNode() );
	GameAssert( skeletonRoot );

	ImportSkeletonRecursive( 1, skeletonRoot, -1, this->modelData );
}

static void ImportKeyframeRecursive( FbxNode* root, const FbxTime& time, Frame& keyFrame )
{
	GameAssert( root );

	FbxAMatrix matrix = root->EvaluateLocalTransform( time );
	FbxVector4 translation = matrix.GetT();
	FbxVector4 rotation = matrix.GetR() * MATH_PI_180;
	FbxVector4 scale = matrix.GetS();

	BoneFrame boneFrame;
	boneFrame.translation.set( (float) translation[0], (float) translation[1], (float) translation[2] );
	boneFrame.rotation.set( ROT_XYZ, (float) rotation[0], (float) rotation[1], (float) rotation[2] );
	boneFrame.scale.set( (float) scale[0], (float) scale[1], (float) scale[2] );
	keyFrame.bones.push_back( boneFrame );

	Logger::Log_Debug( "      Bone: %s", root->GetName() );
	Logger::Log_Debug( "        Translation: (%4.2f, %4.2f, %4.2f)", boneFrame.translation[X], boneFrame.translation[Y], boneFrame.translation[Z] );
	Logger::Log_Debug( "        Rotation: (%1.2f, %1.2f, %1.2f)", rotation[0], rotation[1], rotation[2] );
	Logger::Log_Debug( "        Scale: (%4.2f, %4.2f, %4.2f)", boneFrame.scale[X], boneFrame.scale[Y], boneFrame.scale[Z] );

	for( int i = 0; i < root->GetChildCount(); i++ )
	{
		ImportKeyframeRecursive( root->GetChild( i ), time, keyFrame );
	}
}

void FBXImporter::ImportAnimations()
{
	Logger::Log_Info( "Importing animations..." );

	FbxTime frameTime( FbxTime::GetOneFrameValue( FbxTime::GetGlobalTimeMode() ) );

	for( int stackIndex = 0; stackIndex < this->scene->GetSrcObjectCount<FbxAnimStack>(); stackIndex++ )
	{
		FbxAnimStack* stack = this->scene->GetSrcObject<FbxAnimStack>( stackIndex );
		this->scene->SetCurrentAnimationStack( stack );

		Logger::Log_Debug( "  Animation Name: %s", stack->GetName() );
		Animation anim( stack->GetName() );

		int frameCount = (int) FbxTimeSpan( stack->LocalStart, stack->LocalStop ).GetDuration().GetFrameCount( FbxTime::GetGlobalTimeMode() );
		FbxTime time;
		
		for( int frame = 0; frame < frameCount; frame++ )
		{
			time.SetTime( 0, 0, 0, frame, 0, 0, FbxTime::GetGlobalTimeMode() );

			FbxNode* skeletonRoot = FindNode( FbxNodeAttribute::eSkeleton, this->scene->GetRootNode() );
			GameAssert( skeletonRoot );

			Frame keyFrame;
			keyFrame.frameTime = (unsigned int) time.GetMilliSeconds();
			Logger::Log_Debug( "    Key Frame - Time: %d ms", keyFrame.frameTime );
			ImportKeyframeRecursive( skeletonRoot, time, keyFrame );
			anim.Add_Frame( keyFrame );
		}

		this->modelData.Add_Animation( anim );
	}
}

void FBXImporter::ImportSkin()
{
	Logger::Log_Info( "Importing Skin..." );

	GameAssert( this->scene->GetRootNode() );
	FbxNode* node = FindNode( FbxNodeAttribute::eMesh, this->scene->GetRootNode() );
	GameAssert( node );
	FbxMesh* mesh = static_cast<FbxMesh*>( node->GetNodeAttribute() );
	GameAssert( mesh );

	unsigned int skinCount = mesh->GetDeformerCount( FbxDeformer::eSkin );
	GameCheckFatal( 1 == skinCount, "This converter only supports 1 skin." ); // We will only handle one skin
	
	const FbxSkin* skin = static_cast<FbxSkin*>( mesh->GetDeformer( 0, FbxDeformer::eSkin ) );
	
	unsigned int clusterCount = skin->GetClusterCount();
	const vector<Bone>& skeleton = this->modelData.Get_Bones();

	for( unsigned int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++ )
	{
		const FbxCluster* cluster = skin->GetCluster( clusterIndex );

		for( unsigned int boneIndex = 0; boneIndex < skeleton.size(); boneIndex++ )
		{
			if( cluster->GetLink() && cluster->GetLink()->GetName() == skeleton[boneIndex].name )
			{
				// Usually Identity, but the SDK states that it should also be used
				const FbxNode* node = cluster->GetLink();
				FbxVector4 geometricTranslation = node->GetGeometricTranslation( FbxNode::eSourcePivot );
				FbxVector4 geometricRotation = node->GetGeometricRotation( FbxNode::eSourcePivot );
				FbxVector4 geometricScaling = node->GetGeometricScaling( FbxNode::eSourcePivot );
				FbxAMatrix geometryMatrix( geometricTranslation, geometricRotation, geometricScaling );

				// Node Transform
				FbxAMatrix nodeTransform;
				cluster->GetTransformMatrix( nodeTransform );

				// Link Transform - This should be the bind pose
				FbxAMatrix linkTransform;
				cluster->GetTransformLinkMatrix( linkTransform );

				FbxAMatrix bindPoseMatrix = linkTransform.Inverse() * nodeTransform * geometryMatrix;

				Vect row0( (float) bindPoseMatrix.Get( 0, 0 ), (float) bindPoseMatrix.Get( 0, 1 ), (float) bindPoseMatrix.Get( 0, 2 ), (float) bindPoseMatrix.Get( 0, 3 ) );
				Vect row1( (float) bindPoseMatrix.Get( 1, 0 ), (float) bindPoseMatrix.Get( 1, 1 ), (float) bindPoseMatrix.Get( 1, 2 ), (float) bindPoseMatrix.Get( 1, 3 ) );
				Vect row2( (float) bindPoseMatrix.Get( 2, 0 ), (float) bindPoseMatrix.Get( 2, 1 ), (float) bindPoseMatrix.Get( 2, 2 ), (float) bindPoseMatrix.Get( 2, 3 ) );
				Vect row3( (float) bindPoseMatrix.Get( 3, 0 ), (float) bindPoseMatrix.Get( 3, 1 ), (float) bindPoseMatrix.Get( 3, 2 ), (float) bindPoseMatrix.Get( 3, 3 ) );

				Matrix invBindMatrix( row0, row1, row2, row3 );

				Logger::Log_Debug( "  Bone %s Skin Info:", skeleton[boneIndex].name.c_str() );
				Logger::Log_Debug( "    Inverse Bone Pose:" );
				Logger::Log_Debug( "      [%1.2f %1.2f %1.2f %1.2f]", invBindMatrix[m0], invBindMatrix[m1], invBindMatrix[m2], invBindMatrix[m3] );
				Logger::Log_Debug( "      [%1.2f %1.2f %1.2f %1.2f]", invBindMatrix[m4], invBindMatrix[m5], invBindMatrix[m6], invBindMatrix[m7] );
				Logger::Log_Debug( "      [%1.2f %1.2f %1.2f %1.2f]", invBindMatrix[m8], invBindMatrix[m9], invBindMatrix[m10], invBindMatrix[m11] );
				Logger::Log_Debug( "      [%1.2f %1.2f %1.2f %1.2f]", invBindMatrix[m12], invBindMatrix[m13], invBindMatrix[m14], invBindMatrix[m15] );

				this->modelData.Bind_Bone( boneIndex, invBindMatrix );

				unsigned int vertexCount = cluster->GetControlPointIndicesCount();
				int* vertexIndices = cluster->GetControlPointIndices();
				double* vertexWeights = cluster->GetControlPointWeights();

				Logger::Log_Debug( "    Vertices Influenced: ([Control Point]: weight)" );
				for( unsigned int i = 0; i < vertexCount; i++ )
				{
					BoneInfluence influence;
					influence.boneIndex = boneIndex;
					influence.weight = static_cast<float>( vertexWeights[i] );

					Logger::Log_Debug( "      [CP %d]: %1.2f", vertexIndices[i], influence.weight );

					this->modelData.Add_Influence( vertexIndices[i], influence );
				}

				break;
			}
		}
	}
}
