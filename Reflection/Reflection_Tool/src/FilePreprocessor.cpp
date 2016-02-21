#include <GameAssert.h>
#include <Execute.h>
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

void FilePreprocessor::PreprocessFile( const string& clCommandLine, const string& src2srcmlCommandLine, const string& filename, const char* srcmlFile, FeedbackContext& context )
{
	context.SetCurrentFile( filename.c_str() );

	string cl = clCommandLine + filename;

	char* out;
	char* error;

	unsigned long exitCode = Exec::Execute( cl.c_str(), nullptr, &out, &error );
	
	cout << out;
	cerr << error;
	delete out;
	delete error;

	FEEDBACK_CHECK_RETURN( context, exitCode == 0, MessageType::MSG_TYPE_ERROR, "cl.exe returned an error." );

	exitCode = Exec::Execute( src2srcmlCommandLine.c_str(), nullptr, &out, &error );
	
	cout << out;
	cerr << error;
	delete out;
	delete error;

	FEEDBACK_CHECK_RETURN( context, exitCode == 0, MessageType::MSG_TYPE_ERROR, "srcml.exe returned an error." );

	TiXmlDocument doc( srcmlFile );

	FEEDBACK_CHECK_RETURN( context, doc.LoadFile(), MessageType::MSG_TYPE_ERROR, (std::string("Error ") + std::to_string(doc.ErrorId()) + " when loading XML document: " + doc.ErrorDesc()).c_str() );

	// Write will clean up the data pointer when it is finished with it
	this->out->Write( doc, context );
}