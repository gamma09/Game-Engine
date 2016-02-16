#pragma once

#include <string>

class SrcmlConverter;
class FeedbackContext;

class FilePreprocessor
{
public:
	FilePreprocessor( SrcmlConverter* converter );
	~FilePreprocessor();
	
	// Reads the file with the filename specified and writes the contents
	// to the source code pipe
	void PreprocessFile( const char* filename, FeedbackContext& context );
	
private:
	SrcmlConverter* out;
	
	
private:
	FilePreprocessor( const FilePreprocessor& reader ) = delete;
	FilePreprocessor( FilePreprocessor&& reader ) = delete;
	FilePreprocessor& operator=( const FilePreprocessor& reader ) = delete;
	FilePreprocessor& operator=( FilePreprocessor&& reader ) = delete;
};