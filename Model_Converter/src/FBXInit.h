#pragma once

#include <string>

using namespace std;

#include <fbxsdk.h>

void InitializeSdkObjects( FbxManager*& pManager, FbxScene*& pScene );
void LoadScene( FbxManager* pManager, FbxDocument* pScene, const string& path, const string& filename );