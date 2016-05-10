#pragma once

#include "ModelData.h"

class ModelWriter
{
public:
	// path must end with directory separator
	// Archive file must ONLY be the file name
	// version must be 19 chars or shorter (not counting null-terminator)
	static void Write( const ModelData& data, const char* path, const char* version );


private:
	static void Write_Model( const ModelData& modelData, const char* path );
	static void Write_Texture( const Texture& texture, const char* path );
	static void Write_Manifest( const ModelData& modelData, const char* path );
};