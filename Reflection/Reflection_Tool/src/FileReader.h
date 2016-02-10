#pragma once

#include "SourceCodePipeline.h"

class FileReader : public SourceCodePipeline
{
public:
	FileReader( SourceCodePipeline* outputPipe );
	virtual ~FileReader() override;
	
	
	
	
	
	
private:
	FileReader( const FileReader& reader ) = delete;
	FileReader( FileReader&& reader ) = delete;
	FileReader& operator=( const FileReader& reader ) = delete;
	FileReader& operator=( FileReader&& reader ) = delete;
};