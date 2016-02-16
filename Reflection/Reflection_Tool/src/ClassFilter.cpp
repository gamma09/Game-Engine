#include <string.h>

#include <GameAssert.h>
#include <tinyxml.h>
#include "ClassFilter.h"
#include "Log.h"
#include "FeedbackContext.h"

void Remove( TiXmlElement* element, const char* tagToRemove )
{
	TiXmlElement* curr = element->FirstChildElement( );
	while( curr != nullptr )
	{
		TiXmlElement* next = curr->NextSiblingElement( );
		if( strcmp( curr->Value(), tagToRemove ) == 0 )
		{
			element->RemoveChild( curr );
		}

		curr = next;
	}
}

void RemoveRecursive( TiXmlElement* element, const char* tagToRemove )
{
	Remove( element, tagToRemove );

	TiXmlElement* curr = element->FirstChildElement();
	while( curr != nullptr )
	{
		TiXmlElement* next = curr->NextSiblingElement();
		RemoveRecursive( curr, tagToRemove );
		if( curr->NoChildren() )
		{
			element->RemoveChild( curr );
		}

		curr = next;
	}
}

ClassFilter::ClassFilter( SourceCodePipe* outVal ) :
	out( outVal )
{
	GameAssert( outVal );
}

ClassFilter::~ClassFilter()
{
	delete this->out;
}

void ClassFilter::Write( TiXmlDocument& doc, FeedbackContext& context )
{
	TiXmlElement* unit = doc.RootElement();

	FEEDBACK_CHECK_RETURN( context, unit, MessageType::MSG_TYPE_ERROR, "No root element found in srcml." );
	FEEDBACK_CHECK_RETURN( context, strcmp( unit->Value(), "unit" ) == 0, MessageType::MSG_TYPE_ERROR, "Unknown root element type in srcml: %s", unit->Value() );

	RemoveRecursive( unit, "function" );
	RemoveRecursive( unit, "constructor" );
	RemoveRecursive( unit, "destructor" );
	RemoveRecursive( unit, "class_decl" );
	Remove( unit, "decl_stmt" );

	LOG( "=== ClassFilter::Write ===" );
	LOG( doc );

	this->out->Write( doc, context );
}

void ClassFilter::Finish()
{
	this->out->Finish();
}