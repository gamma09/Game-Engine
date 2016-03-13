#pragma once

class TiXmlDocument;
class FeedbackContext;

// This is an interface for the source code pipeline
class SourceCodePipe
{
protected:
	SourceCodePipe() { }
	SourceCodePipe( const SourceCodePipe& ) { }
	SourceCodePipe( SourceCodePipe&& ) { }
	SourceCodePipe& operator=( const SourceCodePipe& ) { }
	SourceCodePipe& operator=( SourceCodePipe&& ) { }
	
public:
	virtual ~SourceCodePipe() { }
	
	virtual bool Write( TiXmlDocument& doc, FeedbackContext& context ) abstract;
	virtual void Finish() abstract;
};