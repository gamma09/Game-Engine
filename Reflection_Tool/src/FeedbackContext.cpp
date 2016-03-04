#include <stdio.h>
#include <string.h>
#include <stdarg.h>

using namespace std;

#include <GameAssert.h>
#include "FeedbackContext.h"

FeedbackContext::FeedbackContext( std::ostream& outParam ) :
	out( outParam ),
	currentFile( new char[1] ),
	warningCount( 0 ),
	errorCount( 0 )
{
	this->currentFile[0] = 0;
}

FeedbackContext::FeedbackContext( FeedbackContext&& context ) :
	out( context.out ),
	currentFile( context.currentFile ),
	warningCount( context.warningCount ),
	errorCount( context.errorCount )
{

	context.currentFile = nullptr;
}

FeedbackContext::~FeedbackContext()
{
	if( this->currentFile != nullptr )
	{
		delete this->currentFile;
		this->currentFile = nullptr;
	}
}
	
void FeedbackContext::AddMessage( MessageType type, const char* message )
{
	switch( type )
	{
		case MessageType::MSG_TYPE_INFO:
			this->out << "INFO: " << message << endl;
			break;
			
		case MessageType::MSG_TYPE_WARNING:
			this->warningCount += 1;
			this->out << "WARNING: " << this->currentFile << ": " << message << endl;
			break;
		
		case MessageType::MSG_TYPE_ERROR:
			this->errorCount += 1;
			this->out << "ERROR: " << this->currentFile << ": " << message << endl;
			break;
			
		case MessageType::MSG_TYPE_LINK_ERROR:
			this->errorCount += 1;
			this->out << "LINK ERROR: " << message << endl;
			break;
		
		default:
			// Did you forget a message type?
			GameAssert( 0 );
			break;
	}
}

void FeedbackContext::SetCurrentFile( const char* file )
{
	GameAssert( file );
	GameAssert( file != this->currentFile );
	
	delete this->currentFile;
	this->currentFile = new char[strlen( file ) + 1];
	strcpy( this->currentFile, file );
}

unsigned int FeedbackContext::GetNumWarnings() const
{
	return this->warningCount;
}

unsigned int FeedbackContext::GetNumErrors() const
{
	return this->errorCount;
}
