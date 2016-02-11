#pragma once

// This is an interface for the source code pipeline
class SourceCodePipe
{
protected:
	SourceCodePipe() { }
	SourceCodePipe( const SourceCodePipe& pipe ) { }
	SourceCodePipe( SourceCodePipe&& pipe ) { }
	SourceCodePipe& operator=( const SourceCodePipe& pipe ) { }
	SourceCodePipe& operator=( SourceCodePipe&& pipe ) { }
	
public:
	virtual ~SourceCodePipe() { }
	
	virtual void Write( const char* str ) abstract;
	virtual void Finish() abstract;
};