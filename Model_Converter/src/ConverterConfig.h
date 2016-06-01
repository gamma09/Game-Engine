#pragma once

#include <vector>
#include <string>

class ConverterConfig
{
private:
	ConverterConfig();
	~ConverterConfig();

	ConverterConfig( const ConverterConfig& ) = delete;
	ConverterConfig& operator=( const ConverterConfig& ) = delete;

public:
	static ConverterConfig* Instance();

	std::string fbxFile;
	std::vector<std::string> extraTextures;
	
	float keyframe_drop_tolerance_translation;
	float keyframe_drop_tolerance_scale;
	
	// we always represent this in radians
	float keyframe_drop_tolerance_angle;
};