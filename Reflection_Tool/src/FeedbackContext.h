#pragma once

#include <forward_list>
#include <iostream>
#include "Log.h"

enum MessageType
{
	MSG_TYPE_INFO,
	MSG_TYPE_WARNING,
	MSG_TYPE_ERROR,
	MSG_TYPE_LINK_ERROR
};

#define FEEDBACK_CHECK_RETURN_VALUE( returnVal, context, valueToCheck, type, str ) if( !( valueToCheck ) ) { ( context ).AddMessage( type, str ); return returnVal; }
#define FEEDBACK_CHECK_RETURN_VALUE_XML( returnVal, context, valueToCheck, type, str, xml ) if( !( valueToCheck ) ) { ( context ).AddMessage( type, str ); LOG( xml ); return returnVal; }

#define FEEDBACK_CHECK_RETURN( context, valueToCheck, type, str ) if( !( valueToCheck ) ) { ( context ).AddMessage( type, str ); return; }
#define FEEDBACK_CHECK_RETURN_XML( context, valueToCheck, type, str, xml ) if( !( valueToCheck ) ) { ( context ).AddMessage( type, str ); LOG( xml ); return; }

class FeedbackContext
{
public:
	FeedbackContext( std::ostream& out );
	FeedbackContext( FeedbackContext&& context );
	
	~FeedbackContext();
	
	void AddMessage( MessageType type, const char* message );
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
