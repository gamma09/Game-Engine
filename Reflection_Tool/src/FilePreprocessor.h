#pragma once

#include <string>
#include <vector>

using namespace std;

class SourceCodePipe;
class FeedbackContext;

class FilePreprocessor
{
public:
	FilePreprocessor( SourceCodePipe* pipeOut );
	~FilePreprocessor();
	
	// Reads the file with the filename specified and writes the contents
	// to the source code pipe
	bool PreprocessFile( const string& clCommandLine, const string& src2srcmlCommandLine, const string& filename, const char* srcmlFile, FeedbackContext& context );
	
private:
	SourceCodePipe* out;
	
	
private:
	FilePreprocessor( const FilePreprocessor& reader ) = delete;
	FilePreprocessor( FilePreprocessor&& reader ) = delete;
	FilePreprocessor& operator=( const FilePreprocessor& reader ) = delete;
	FilePreprocessor& operator=( FilePreprocessor&& reader ) = delete;
};