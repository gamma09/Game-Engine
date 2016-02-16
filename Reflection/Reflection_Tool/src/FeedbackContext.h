#pragma once

#include <forward_list>
#include <iostream>

enum MessageType
{
	MSG_TYPE_INFO,
	MSG_TYPE_WARNING,
	MSG_TYPE_ERROR,
	MSG_TYPE_LINK_ERROR
};

#define FEEDBACK_CHECK_RETURN_VALUE( returnVal, context, valueToCheck, type, format, ... ) if( !(valueToCheck) ) { (context).AddMessage( type, format, __VA_ARGS__ ); return returnVal; }
#define FEEDBACK_CHECK_RETURN( context, valueToCheck, type, format, ... ) if( !(valueToCheck) ) { (context).AddMessage( type, format, __VA_ARGS__ ); return; }

class FeedbackContext
{
public:
	FeedbackContext( std::ostream& out );
	FeedbackContext( FeedbackContext&& context );
	
	~FeedbackContext();
	
	void AddMessage( MessageType type, const char* messageFormat, ... );
	void SetCurrentFile( const char* file );
	unsigned int GetNumWarnings() const;
	unsigned int GetNumErrors() const;
	
private:
	char* currentFile;
	
	std::ostream& out;
	
	unsigned int warningCount;
	unsigned int errorCount;
	
	
	
private:
	FeedbackContext( const FeedbackContext& context ) = delete;
	FeedbackContext& operator=( const FeedbackContext& context ) = delete;
	FeedbackContext& operator=( FeedbackContext&& context ) = delete;
};
