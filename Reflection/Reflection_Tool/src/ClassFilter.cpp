#include <string.h>
#include <set>

#include <GameAssert.h>
#include <tinyxml.h>
#include "ClassFilter.h"
#include "Log.h"
#include "FeedbackContext.h"

using namespace std;

void Remove( TiXmlElement* element, const set<string>& tagsToRemove )
{
	TiXmlElement* curr = element->FirstChildElement( );
	while( curr != nullptr )
	{
		TiXmlElement* next = curr->NextSiblingElement( );
		if( tagsToRemove.find(curr->ValueStr()) != tagsToRemove.cend() )
		{
			element->RemoveChild( curr );
		}

		curr = next;
	}
}

void RemoveRecursive( TiXmlElement* element, const set<string>& tagsToRemove )
{
	Remove( element, tagsToRemove );

	TiXmlElement* curr = element->FirstChildElement();
	while( curr != nullptr )
	{
		TiXmlElement* next = curr->NextSiblingElement();
		RemoveRecursive( curr, tagsToRemove );
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

	FEEDBACK_CHECK_RETURN_XML( context, unit, MessageType::MSG_TYPE_ERROR, "No root element found in srcml.", doc );
	FEEDBACK_CHECK_RETURN_XML( context, strcmp( unit->Value(), "unit" ) == 0, MessageType::MSG_TYPE_ERROR, (string("Unknown root element type in srcml: ") + unit->Value() ).c_str(), *unit );

	set<string> recursiveRemovals;
	recursiveRemovals.emplace( "function" );
	recursiveRemovals.emplace( "constructor" );
	recursiveRemovals.emplace( "destructor" );
	recursiveRemovals.emplace( "class_decl" );

	RemoveRecursive( unit, recursiveRemovals );

	set<string> regularRemovals;
	regularRemovals.emplace( "decl_stmt" );
	Remove( unit, regularRemovals );

	this->out->Write( doc, context );
}

void ClassFilter::Finish()
{
	this->out->Finish();
}