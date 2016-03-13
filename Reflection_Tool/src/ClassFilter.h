#pragma once

#include "SourceCodePipe.h"

// This class filters out all document nodes that are not classes/structs
class ClassFilter : public SourceCodePipe
{
public:
	ClassFilter( SourceCodePipe* out );
	virtual ~ClassFilter();

	virtual bool Write( TiXmlDocument& doc, FeedbackContext& context ) override;
	virtual void Finish() override;

private:
	SourceCodePipe* out;



private:
	ClassFilter( const ClassFilter& ) = delete;
	ClassFilter( ClassFilter&& ) = delete;
	ClassFilter& operator=( const ClassFilter& ) = delete;
	ClassFilter& operator=( ClassFilter&& ) = delete;
};