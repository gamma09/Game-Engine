#pragma once

// This is an interface for the source code pipeline
class SourceCodePipeline
{
protected:
	SourceCodePipeline() { }
	SourceCodePipeline( const SourceCodePipeline& pipeline ) { }
	SourceCodePipeline( SourceCodePipeline&& pipeline ) { }
	SourceCodePipeline& operator=( const SourceCodePipeline& pipeline ) { }
	SourceCodePipeline& operator=( SourceCodePipeline&& pipeline ) { }
	
public:
	virtual ~SourceCodePipeline() { }
	
	virtual void Write( const char* str ) abstract;
	virtual void Finish() abstract;
};