#include <GameAssert.h>
#include <File.h>
#include <tinyxml.h>

#include "FilePreprocessor.h"
#include "SourceCodePipe.h"
#include "Log.h"
#include "FeedbackContext.h"

FilePreprocessor::FilePreprocessor( SourceCodePipe* outputPipe )
:	out( outputPipe )
{
	GameAssert( outputPipe );
}

FilePreprocessor::~FilePreprocessor()
{
	this->out->Finish();
	delete this->out;
}

void FilePreprocessor::ReadFile( const char* filename, FeedbackContext& context )
{
	GameAssert( filename );

	context.AddMessage( MessageType::MSG_TYPE_INFO, filename );
	context.SetCurrentFile( filename );

	TiXmlDocument doc( filename );

	FEEDBACK_CHECK_RETURN( context, doc.LoadFile(), MessageType::MSG_TYPE_ERROR, "Error %d when loading XML document: %s", doc.ErrorId(), doc.ErrorDesc() )

	LOG( "=== FilePreprocessor::ReadFile ===" );
	LOG( doc );

	// Write will clean up the data pointer when it is finished with it
	this->out->Write( doc, context );
}