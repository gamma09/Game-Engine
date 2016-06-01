#include "FBXInit.h"

#include <GameAssert.h>

void InitializeSdkObjects( FbxManager*& pManager, FbxScene*& pScene )
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	GameAssert( pManager != 0 );

	printf( "Autodesk FBX SDK version %s\n", pManager->GetVersion() );

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create( pManager, IOSROOT );
	pManager->SetIOSettings( ios );

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create( pManager, "My Scene" );
	GameAssert( pScene != 0 );
}



#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif



void LoadScene( FbxManager* pManager, FbxDocument* pScene, const string& path, const string& filename )
{
	bool lStatus;

	// Get the file version number generate by the FBX SDK
	int lSDKMajor, lSDKMinor, lSDKRevision;
	FbxManager::GetFileFormatVersion( lSDKMajor, lSDKMinor, lSDKRevision );

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create( pManager, "" );

	// generate the full file name
	string filePath = path + filename;

	// Initialize the importer by providing a filename	
	const bool lImportStatus = lImporter->Initialize( filePath.c_str(), -1, pManager->GetIOSettings() );

	// Get the the file version number of the fbx file
	int lFileMajor, lFileMinor, lFileRevision;
	lImporter->GetFileVersion( lFileMajor, lFileMinor, lFileRevision );

	if( !lImportStatus )
	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		fprintf( stderr, "Call to FbxImporter::Initialize() failed.\n" );
		fprintf( stderr, "Error returned: %s\n\n", error.Buffer() );

		if( lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion )
		{
			fprintf( stderr, "FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision );
			fprintf( stderr, "FBX file format version for file '%s' is %d.%d.%d\n\n", filePath.c_str(), lFileMajor, lFileMinor, lFileRevision );
		}

		exit( 1 );
	}

	if( !lImporter->IsFBX() )
	{
		fprintf( stderr, "Error: file is not in FBX format\n" );
		exit( 1 );
	}

	// Set the import states. By default, the import states are always set to 
	// true. The code below shows how to change these states.
	IOS_REF.SetBoolProp( IMP_FBX_MATERIAL, true );
	IOS_REF.SetBoolProp( IMP_FBX_TEXTURE, true );
	IOS_REF.SetBoolProp( IMP_FBX_LINK, true );
	IOS_REF.SetBoolProp( IMP_FBX_SHAPE, true );
	IOS_REF.SetBoolProp( IMP_FBX_GOBO, true );
	IOS_REF.SetBoolProp( IMP_FBX_ANIMATION, true );
	IOS_REF.SetBoolProp( IMP_FBX_GLOBAL_SETTINGS, true );


	// Import the scene.
	lStatus = lImporter->Import( pScene );

	if( lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError )
	{
		fprintf( stderr, "Please enter password: " );

		char lPassword[1024];
		lPassword[0] = '\0';

		scanf_s( "%s", lPassword, _countof( lPassword ) );

		FbxString lString( lPassword );

		IOS_REF.SetStringProp( IMP_FBX_PASSWORD, lString );
		IOS_REF.SetBoolProp( IMP_FBX_PASSWORD_ENABLE, true );

		lStatus = lImporter->Import( pScene );

		if( lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError )
		{
			fprintf( stderr, "\nPassword is wrong, import aborted.\n" );
			exit( 1 );
		}
	}

	// Destroy the importer.
	lImporter->Destroy();
}