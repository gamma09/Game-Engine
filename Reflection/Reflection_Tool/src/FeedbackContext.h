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

class FeedbackContext
{
public:
	FeedbackContext( std::ostream& out );
	FeedbackContext( FeedbackContext&& context );
	FeedbackContext& operator=( FeedbackContext&& context );
	~FeedbackContext();
	
	void AddMessage( MessageType type, const char* messageFormat, ... );
	void SetCurrentFile( const char* file );
	void SetCurrentLine( unsigned int line );
	unsigned int GetNumWarnings() const;
	unsigned int GetNumErrors() const;
	
private:
	char* currentFile;
	unsigned int currentLine;
	
	std::ostream& out;
	
	unsigned int warningCount;
	unsigned int errorCount;
	
	
	
private:
	FeedbackContext( const FeedbackContext& context ) = delete;
	FeedbackContext& operator=( const FeedbackContext& context ) = delete;
};
