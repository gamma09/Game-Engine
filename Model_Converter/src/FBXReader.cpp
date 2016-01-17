#include "FBXReader.h"

#include <GameAssert.h>
#include <stack>

#include "FBXInit.h"
#include "ModelData.h"


void FBXReader::Read( ModelData& modelData, const char* path, const char* fbxFilename )
{
	FbxManager* fbxManager = 0;
	FbxScene* scene = 0;

	InitializeSdkObjects( fbxManager, scene );
	LoadScene( fbxManager, scene, path, fbxFilename );

	FbxGeometryConverter converter( fbxManager );
	GameVerify( converter.Triangulate( scene, true ) );

	Read_Hierarchy( modelData, scene );
	Read_Animations( modelData, scene );
}




struct Node
{
	Node( FbxNode* node, const char* name, const int level ) :
	node( node ),
	name( name ),
	level( level )
	{
		// Do nothing
	}

	FbxNode* node;
	const char* name;
	int level;
};

void FBXReader::Read_Hierarchy( ModelData& modelData, FbxScene* scene )
{
	FbxNode* node = scene->GetRootNode();
	stack<Node> nodeStack;
	if( node != 0 )
	{
		int childCount = node->GetChildCount();
		for( int i = 0; i < childCount; i++ )
			nodeStack.push( Node( node->GetChild( i ), 0, 0 ) );
	}

	while( !nodeStack.empty() )
	{
		Node parent = nodeStack.top();
		nodeStack.pop();

		FbxNode* currentNode = parent.node;
		if( currentNode == 0 )
			continue;

		FbxNodeAttribute* element = currentNode->GetNodeAttribute();
		if( element == 0 )
			continue;

		FbxNodeAttribute::EType type = element->GetAttributeType();

		int nextLevel = parent.level;
		const char* nextParentName = parent.name;

		if( type == FbxNodeAttribute::eMesh )
		{
			Read_Mesh( modelData, currentNode->GetName(), currentNode );
		}
		else if( type == FbxNodeAttribute::eSkeleton )
		{
			Read_Bone( modelData, parent.name, currentNode, parent.level );
			nextLevel++;
			nextParentName = currentNode->GetName();
		}

		int childCount = currentNode->GetChildCount();
		for( int i = 0; i < childCount; i++ )
		{
			FbxNode* childNode = currentNode->GetChild( i );
			nodeStack.push( Node( childNode, nextParentName, nextLevel ) );
		}
	}
}

void FBXReader::Read_Mesh( ModelData& modelData, const char* name, FbxNode* meshNode )
{
	static bool hasMesh = false;

	if( !hasMesh )
	{

		FbxMesh* mesh = (FbxMesh*) meshNode->GetNodeAttribute();
		GameVerify( mesh->GenerateNormals( true, false, false ) );

#ifdef DEBUG
		fprintf( stdout, "Mesh name: %s\n", name );
#endif

		char title[20];
		memset( title, 0, 20 );
		for( int j = min( 18, strlen( name ) ); j >= 0; j-- )
			title[j] = name[j];

		modelData.Set_Title( title );

#ifdef DEBUG
		fprintf( stdout, "    Polygons\n" );
#endif 

		Read_Triangles( modelData, mesh );

#ifdef DEBUG
		fprintf( stdout, "\n" );
		fprintf( stdout, "    Materials\n" );
#endif

		Read_Textures( modelData, mesh );

		hasMesh = true;
	}
}






void FBXReader::Read_Bone( ModelData& modelData, const char* parentName, FbxNode* boneNode, int level )
{
	const char* name = boneNode->GetName();
	level;

#ifdef DEBUG
	for( int i = 0; i < level; i++ )
		fprintf( stdout, "  " );

	fprintf( stdout, "Bone: %s\n", name );
#endif

	modelData.Get_Animation_Info().Add_Bone( name, parentName, boneNode );
}








void FBXReader::Read_Textures( ModelData& modelData, FbxMesh* mesh )
{
	FbxNode* node = mesh->GetNode();
	int materialCount = node->GetMaterialCount();

	for( int i = 0; i < materialCount; i++ )
	{
		FbxSurfaceMaterial* material = node->GetMaterial( i );

#ifdef DEBUG
		fprintf( stdout, "        Material %d\n", i );
		fprintf( stdout, "            Name: \"%s\"\n", material->GetName() );
		fprintf( stdout, "            Textures:\n" );
#endif

		for( int j = 0; j < FbxLayerElement::sTypeTextureCount; j++ )
		{
			FbxProperty prop = material->FindProperty( FbxLayerElement::sTextureChannelNames[j] );
			GameAssert( prop.IsValid() );

			int textureCount = prop.GetSrcObjectCount<FbxTexture>();
			if( prop.GetSrcObjectCount<FbxLayeredTexture>() != 0 )
			{
				fprintf( stderr, "Error: Layered textures are not supported.\n" );
				exit( 1 );
			}

			for( int k = 0; k < textureCount; k++ )
			{
#ifdef DEBUG
				fprintf( stdout, "                Texture %d\n", k );
#endif
				FbxFileTexture* texture = FbxCast<FbxFileTexture>( prop.GetSrcObject<FbxTexture>( k ) );
				if( texture == 0 )
				{
					fprintf( stderr, "Error: Only file-based textures are supported.\n" );
					exit( 1 );
				}

				char name[20];
				memset( name, 0, 20 );
				const char* nameStr = texture->GetName();
				for( int i = min( 18, strlen( nameStr ) ); i >= 0; i-- )
					name[i] = nameStr[i];

				const char* filename = texture->GetFileName();
				const char* ptr = strrchr( filename, '\\' );
				if( ptr != 0 )
					filename = ptr + 1;

#ifdef DEBUG
				fprintf( stdout, "                    Name: \"%s\"\n", name );
				fprintf( stdout, "                    File: \"%s\"\n", filename );
#endif

				modelData.Add_Texture( name, filename );
			}
		}
	}

#ifdef DEBUG
	fprintf( stdout, "\n" );
	fprintf( stdout, "        Polygon to Material Mappings\n" );
#endif

	int polygonCount = mesh->GetPolygonCount();

	// so far as I know, there can only be one polygon-to-material mapping?
	// Or perhaps the number of material elements equals the number of texture channels?
	// Or perhaps the number of material elements equals the number of different ways to skin (texture) the cat?
	int count = mesh->GetElementMaterialCount();
	GameAssert( count <= 1 );
	for( int i = 0; i < count; i++ )
	{
		FbxGeometryElementMaterial* leMat = mesh->GetElementMaterial( i );
		GameAssert( leMat );

		FbxLayerElement::EMappingMode mapMode = leMat->GetMappingMode();
		FbxLayerElement::EReferenceMode refMode = leMat->GetReferenceMode();

#ifdef DEBUG
		fprintf( stdout, "            Polygon-to-Material Mapping %d:\n", i );
#endif

		if( refMode == FbxGeometryElement::eIndex || refMode == FbxGeometryElement::eIndexToDirect )
		{
			if( mapMode == FbxGeometryElement::eAllSame )
			{
				if( leMat->GetIndexArray().GetCount() == 0 )
				{
					fprintf( stderr, "Error: no material is specified in the polygon-to-material mapping\n" );
					exit( 1 );
				}

				int materialID = leMat->GetIndexArray().GetAt( 0 );
				for( int j = 0; j < polygonCount; j++ )
				{
#ifdef DEBUG
					fprintf( stdout, "                Polygon %5d uses Material %d\n", j, materialID );
#endif
					modelData.Set_Triangle_Texture( j, (short) materialID );
				}
			}
			else if( mapMode == FbxGeometryElement::eByPolygon )
			{

				auto mapping = leMat->GetIndexArray();
				if( mapping.GetCount() != polygonCount )
				{
					fprintf( stderr, "Error: only %d polygons (out of %d) have associated materials.\n", mapping.GetCount(), polygonCount );
					exit( 1 );
				}

				for( int j = 0; j < polygonCount; j++ )
				{
#ifdef DEBUG
					fprintf( stdout, "                Polygon %5d uses Material %d\n", j, mapping.GetAt( j ) );
#endif
					modelData.Set_Triangle_Texture( j, (short) mapping.GetAt( j ) );
				}
			}
			else
				// None of the other mapping modes is allowed for file materials
				// (if this assert happens, it means the documentation of FbxLayerElementMaterial is wrong)
				GameAssert( 0 );
		}
		else // refMode == FbxGeometryElement::eDirect   ---> this is not allowed! It means that FBX doesn't work the way we think it does.
			GameAssert( 0 );
	}
}








void FBXReader::Read_Triangles( ModelData& modelData, FbxMesh* mesh )
{
	FbxVector4* controlPoints = mesh->GetControlPoints();
	int polygonCount = mesh->GetPolygonCount();
	for( int i = 0; i < polygonCount; i++ )
	{
		int vtxCount = mesh->GetPolygonSize( i );
		if( vtxCount != 3 )
		{
			fprintf( stderr, "Error: Only triangles are supported. Please tesselate polygon %d (%d sides) before converting.\n", i, vtxCount );
			exit( 1 );
		}

#ifdef DEBUG
		fprintf( stdout, "        Triangle %d:\n", i );
#endif

		Vertex v0 = Read_Vertex( mesh, controlPoints, i, 0 );
		Vertex v1 = Read_Vertex( mesh, controlPoints, i, 1 );
		Vertex v2 = Read_Vertex( mesh, controlPoints, i, 2 );

		modelData.Add_Triangle( v0, v1, v2 );
	}
}








void Get_UV_From_Control_Point( FbxGeometryElementUV* leUV, const int controlPointIndex, float& u, float& v )
{
	switch( leUV->GetReferenceMode() )
	{
	case FbxGeometryElement::eDirect:
	{
										double* uv = (double*) leUV->GetDirectArray().GetAt( controlPointIndex );
										u = (float) uv[0];
										v = (float) uv[1];
										break;
	}

	case FbxGeometryElement::eIndexToDirect:
	{
											   int id = leUV->GetIndexArray().GetAt( controlPointIndex );
											   double* uv = (double*) leUV->GetDirectArray().GetAt( id );
											   u = (float) uv[0];
											   v = (float) uv[1];
											   break;
	}

	default:
		break;
	}
}








void Get_UV_From_Polygon_Vertex( FbxMesh* mesh, FbxGeometryElementUV* leUV, const int polygon, const int vertex, float& u, float& v )
{
	int uvIndex = mesh->GetTextureUVIndex( polygon, vertex );
	switch( leUV->GetReferenceMode() )
	{
	case FbxGeometryElement::eDirect:
	case FbxGeometryElement::eIndexToDirect:
	{
											   double* uv = (double*) leUV->GetDirectArray().GetAt( uvIndex );
											   u = (float) uv[0];
											   v = (float) uv[1];
											   break;
	}

	default:
		break;
	}
}








Vertex FBXReader::Read_Vertex( FbxMesh* mesh, FbxVector4* controlPoints, const int polygon, const int vertex )
{
	int vertexId = polygon * 3 + vertex;

	Vertex vtx;
	int cpIndex = mesh->GetPolygonVertex( polygon, vertex );
	double* coord = (double*) controlPoints[cpIndex];
	vtx.x = (float) coord[0];
	vtx.y = (float) coord[1];
	vtx.z = (float) coord[2];

	int uvCount = mesh->GetElementUVCount();
	for( int i = 0; i < uvCount; i++ )
	{
		FbxGeometryElementUV* leUV = mesh->GetElementUV( i );

		switch( leUV->GetMappingMode() )
		{
		case FbxGeometryElement::eByControlPoint:
			Get_UV_From_Control_Point( leUV, cpIndex, vtx.u, vtx.v );
			break;

		case FbxGeometryElement::eByPolygonVertex:
			Get_UV_From_Polygon_Vertex( mesh, leUV, polygon, vertex, vtx.u, vtx.v );
			break;

		default:
			break;
		}
	}

	int normalCount = mesh->GetElementNormalCount();
	for( int i = 0; i < normalCount; i++ )
	{
		FbxGeometryElementNormal* leNormal = mesh->GetElementNormal( i );

		if( leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex )
		{
			switch( leNormal->GetReferenceMode() )
			{
			case FbxGeometryElement::eDirect:
			{
												double* n = (double*) leNormal->GetDirectArray().GetAt( vertexId );
												vtx.nx = (float) n[0];
												vtx.ny = (float) n[1];
												vtx.nz = (float) n[2];
												break;
			}

			case FbxGeometryElement::eIndexToDirect:
			{
													   int id = leNormal->GetIndexArray().GetAt( vertexId );
													   double* n = (double*) leNormal->GetDirectArray().GetAt( id );
													   vtx.nx = (float) n[0];
													   vtx.ny = (float) n[1];
													   vtx.nz = (float) n[2];
													   break;
			}

			default:
				break;
			}
		}
	}

#ifdef DEBUG
	fprintf( stdout, "            Vertex:\n" );
	fprintf( stdout, "                Coordinates: (%4.2f, %4.2f, %4.2f)\n", vtx.x, vtx.y, vtx.z );
	fprintf( stdout, "                UV: (%1.2f, %1.2f)\n", vtx.u, vtx.v );
	fprintf( stdout, "                Normal: (%1.2f, %1.2f, %1.2f)\n", vtx.nx, vtx.ny, vtx.nz );
#endif

	return vtx;
}







void FBXReader::Read_Animations( ModelData& modelData, FbxScene* scene )
{
	FbxTime frameTime( FbxTime::GetOneFrameValue( FbxTime::GetGlobalTimeMode() ) );

	for( int i = 0; i < scene->GetSrcObjectCount<FbxAnimStack>(); i++ )
	{
		FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>( i );
		scene->SetCurrentAnimationStack( animStack );

#ifdef DEBUG
		fprintf( stdout, "Animation Name: %s\n", animStack->GetName() );
#endif

		Animation& anim = modelData.Get_Animation_Info().Add_Animation( animStack->GetName() );

		int frameCount = (int) FbxTimeSpan( animStack->LocalStart, animStack->LocalStop ).GetDuration().GetFrameCount( FbxTime::GetGlobalTimeMode() );

		const vector<Bone>& bones = modelData.Get_Animation_Info().Get_Hierarchy();
		FbxTime keyTime;
		for( int j = 0; j <= frameCount; j++ )
		{
			keyTime.SetTime( 0, 0, 0, j, 0, 0, FbxTime::GetGlobalTimeMode() );

#ifdef DEBUG
			fprintf( stdout, "    Key Frame %d - %d ms\n", j, (int) keyTime.GetMilliSeconds() );
#endif

			Frame frame;
			frame.frameTime = (unsigned int) keyTime.GetMilliSeconds();

			for( unsigned int k = 0; k < bones.size(); k++ )
			{
				FbxAMatrix matrix = bones[k].node->EvaluateLocalTransform( keyTime );

				FbxVector4 translation = matrix.GetT();
				FbxVector4 rotation = matrix.GetR() * MATH_PI_180;
				FbxVector4 scale = matrix.GetS();

#ifdef DEBUG
				fprintf( stdout, "        Bone: %s\n", bones[k].name.c_str() );
				fprintf( stdout, "            Translation = [%.2f, %.2f, %.2f]\n", translation[0], translation[1], translation[2] );
				fprintf( stdout, "            Rotation    = [%.2f, %.2f, %.2f]\n", rotation[0], rotation[1], rotation[2] );
				fprintf( stdout, "            Scale       = [%.2f, %.2f, %.2f]\n", scale[0], scale[1], scale[2] );
#endif

				Vect t( (float) translation[0], (float) translation[1], (float) translation[2] );
				Quat r( ROT_XYZ, (float) rotation[0], (float) rotation[1], (float) rotation[2] );
				Vect s( (float) scale[0], (float) scale[1], (float) scale[2] );

				BoneFrame boneFrame;
				boneFrame.translation = t;
				boneFrame.rotation = r;
				boneFrame.scale = s;

				frame.bones.push_back( boneFrame );
			}

			anim.Add_Frame( frame );
		}
	}
}