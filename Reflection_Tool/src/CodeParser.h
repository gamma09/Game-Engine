#pragma once

#include "SourceCodePipe.h"

class ReflectionData;
class TiXmlElement;

class CodeParser : public SourceCodePipe
{
public:
	CodeParser( ReflectionData* data );
	virtual ~CodeParser();

	virtual bool Write( TiXmlDocument& doc, FeedbackContext& context ) override;
	virtual void Finish( ) override;


private:
	// Returns true if successful. Returns false if an error was found.
	bool ReadUnit( TiXmlElement* unitElement, FeedbackContext& context, const char* namespaceSoFar );
	// Returns true if successful (even if no reflected class was found). Returns false if an error was found.
	bool ReadClass( TiXmlElement* classElement, FeedbackContext& context, const char* namespaceSoFar );
	// Returns true if successful read. Returns false if an error was found.
	bool ReadParents( TiXmlElement* superElement, FeedbackContext& context, const char*& parentOut );


private:
	ReflectionData* data;


private:
	CodeParser( const CodeParser& ) = delete;
	CodeParser( CodeParser&& ) = delete;
	CodeParser& operator=( const CodeParser& ) = delete;
	CodeParser& operator=( CodeParser&& ) = delete;
};