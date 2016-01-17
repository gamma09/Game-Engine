#pragma once

#pragma warning( push, 3 )
#include <fbxsdk.h>
#pragma warning( pop )

void InitializeSdkObjects( FbxManager*& pManager, FbxScene*& pScene );
void LoadScene( FbxManager* pManager, FbxDocument* pScene, const char* path, const char* filename );