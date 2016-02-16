#include <GameAssert.h>
#include <tinyxml.h>
#include "CodeParser.h"
#include "ReflectionData.h"
#include "FeedbackContext.h"

CodeParser::CodeParser( ReflectionData* inData ) :
	data( inData )
{
	GameAssert( inData );
}

CodeParser::~CodeParser()
{
	// Do nothing
}

void CodeParser::Write( TiXmlDocument& doc, FeedbackContext& context )
{
	TiXmlElement* unit = doc.RootElement( );
	FEEDBACK_CHECK_RETURN( context, unit, MessageType::MSG_TYPE_ERROR, "No root element found in srcml." );
	FEEDBACK_CHECK_RETURN( context, strcmp( unit->Value( ), "unit" ) == 0, MessageType::MSG_TYPE_ERROR, "Unknown root element type in srcml: %s", unit->Value( ) );

	ReadUnit( unit, context, "" );

	
}

bool CodeParser::ReadUnit( TiXmlElement* unit, FeedbackContext& context, const char* namespaceSoFar )
{
	TiXmlElement* unitElement = unit->FirstChildElement( );
	while( unitElement != nullptr )
	{
		if( strcmp( unitElement->Value( ), "namespace" ) == 0 )
		{
			FEEDBACK_CHECK_RETURN_VALUE( false, context, unitElement->FirstChildElement( "name" ), MessageType::MSG_TYPE_ERROR, "Anonymous namespace not allowed." );
			FEEDBACK_CHECK_RETURN_VALUE( false, context, unitElement->FirstChildElement( "name" )->GetText(), MessageType::MSG_TYPE_ERROR, "Anonymous namespace not allowed." );
			char* newNamespaceSoFar = new char[strlen( namespaceSoFar ) + 2 + strlen( unitElement->FirstChildElement( "name" )->GetText() ) + 1];
			sprintf( newNamespaceSoFar, "%s::%s", namespaceSoFar, unitElement->FirstChildElement( "name" )->GetText() );

			FEEDBACK_CHECK_RETURN_VALUE( false, context, unitElement->FirstChildElement( "block" ), MessageType::MSG_TYPE_ERROR, "Namespace %s must have content block.", newNamespaceSoFar );
			bool retVal = ReadUnit( unitElement->FirstChildElement( "block" ), context, newNamespaceSoFar );
			delete newNamespaceSoFar;

			if( !retVal ) return retVal;
		}
		else if( strcmp( unitElement->Value(), "class" ) == 0 || strcmp( unitElement->Value(), "struct" ) == 0 )
		{
			if( !ReadClass( unitElement, context, namespaceSoFar ) ) break;
		}

		unitElement = unitElement->NextSiblingElement();
	}

	return true;
}

bool CodeParser::ReadClass( TiXmlElement* classElement, FeedbackContext& context, const char* namespaceSoFar )
{
	GameAssert( classElement );
	GameAssert( strcmp( classElement->Value(), "class" ) == 0 || strcmp( classElement->Value(), "struct" ) == 0 );

	TiXmlElement* nameElement = classElement->FirstChildElement( "name" );
	FEEDBACK_CHECK_RETURN_VALUE( false, context, nameElement, MessageType::MSG_TYPE_ERROR, "Unexpected end of class/struct tag." );

	const char* className = nameElement->GetText();
	FEEDBACK_CHECK_RETURN_VALUE( false, context, className, MessageType::MSG_TYPE_ERROR, "Class name not found." );

	const char* reflectedParent;
	if( !ReadParents( classElement->FirstChildElement( "super" ), context, reflectedParent ) ) return false;

	TiXmlElement* classBlock = classElement->FirstChildElement( "block" );
	FEEDBACK_CHECK_RETURN_VALUE( true, context, classBlock, MessageType::MSG_TYPE_WARNING, "Class %s has no block tag.", className );

	ReflectedType* reflectedType = nullptr;

	TiXmlElement* accessModifier = classBlock->FirstChildElement();
	while( accessModifier != nullptr )
	{
		bool isPrivate = strcmp( accessModifier->Value(), "private" ) == 0;
		bool isPublic = strcmp( accessModifier->Value(), "public" ) == 0;
		bool isProtected = strcmp( accessModifier->Value(), "protected" ) == 0;
		FEEDBACK_CHECK_RETURN_VALUE( false, context, isPrivate || isPublic || isProtected, MessageType::MSG_TYPE_ERROR, "Unknown class storage modifier: %s.", accessModifier->Value() );

		TiXmlElement* blockElement = accessModifier->FirstChildElement( "block" );
		while( blockElement != nullptr )
		{
			if( strcmp( blockElement->Value(), "block" ) == 0 )
			{
				TiXmlElement* labelElement = blockElement->FirstChildElement( "label" );
				if( labelElement != nullptr && labelElement->FirstChildElement( "name" ) != nullptr )
				{
					if( strcmp( labelElement->FirstChildElement( "name" )->GetText(), "CLASS_IS_REFLECTED" ) == 0 )
					{
						TiXmlElement* exprStmt = blockElement->FirstChildElement( "expr_stmt" );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, exprStmt, MessageType::MSG_TYPE_ERROR, "Missing expr_stmt block for reflected class %s.", className );

						TiXmlElement* expr = exprStmt->FirstChildElement( "expr" );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, expr, MessageType::MSG_TYPE_ERROR, "Missing expr block for reflected class %s.", className );

						TiXmlElement* name = expr->FirstChildElement( "name" );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, name, MessageType::MSG_TYPE_ERROR, "Missing class name block for reflected class %s.", className );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, name->GetText(), MessageType::MSG_TYPE_ERROR, "Missing class name string for reflected class %s.", className );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, strcmp( name->GetText(), className ) == 0, MessageType::MSG_TYPE_ERROR, "Mismatch: Actual class name is %s, but the reflected class name is %s.", className, name->GetText() );

						char* fullClassName = new char[strlen( namespaceSoFar ) + strlen( className ) + 1];
						sprintf( fullClassName, "%s%s", namespaceSoFar, className );

						reflectedType = this->data->AddType( fullClassName, context );
						delete fullClassName;

						if( reflectedParent != nullptr )
						{
							reflectedType->SetParentType( reflectedParent, context );
						} // End if reflectedParent != nullptr
					} // End if label == CLASS_IS_REFLECTED
					else if( strcmp( labelElement->FirstChildElement( "name" )->GetText(), "VARIABLE_IS_REFLECTED" ) == 0 )
					{
						FEEDBACK_CHECK_RETURN_VALUE( false, context, reflectedType, MessageType::MSG_TYPE_ERROR, "Reflected variable declared before class reflection declaration in class %s", className );

						TiXmlElement* declStmt = blockElement->FirstChildElement( "decl_stmt" );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, declStmt, MessageType::MSG_TYPE_ERROR, "Bad reflection variable declaration srcml block for class %s.", className );

						TiXmlElement* decl = declStmt->FirstChildElement( "decl" );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, decl, MessageType::MSG_TYPE_ERROR, "Bad reflection variable declaration srcml block for class %s.", className );

						TiXmlElement* type = decl->FirstChildElement( "type" );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, type, MessageType::MSG_TYPE_ERROR, "Bad reflection variable declaration srcml block for class %s.", className );

						TiXmlElement* name = type->FirstChildElement( "name" );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, name, MessageType::MSG_TYPE_ERROR, "Bad reflection variable declaration srcml block for class %s.", className );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, name->GetText(), MessageType::MSG_TYPE_ERROR, "Bad reflection variable declaration srcml block for class %s.", className );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, this->data->FindType( name->GetText() ), MessageType::MSG_TYPE_ERROR, "Cannot declare a variable as reflected if its class type is not reflected itself. Variable type %s in class %s", name->GetText(), className );

						const char* variableTypename = name->GetText();

						TiXmlElement* valueNameElement = decl->FirstChildElement( "name" );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, valueNameElement, MessageType::MSG_TYPE_ERROR, "Bad reflection variable declaration srcml block for class %s.", className );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, valueNameElement->GetText( ), MessageType::MSG_TYPE_ERROR, "Bad reflection variable declaration srcml block for class %s.", className );

						const char* variableIdentifier = valueNameElement->GetText();

						reflectedType->AddVariable( variableIdentifier, variableTypename, context );
					} // End if label == VARIABLE_IS_REFLECTED
					else if( strcmp( labelElement->FirstChildElement( "name" )->GetText(), "CLASS_FILE" ) == 0 )
					{
						TiXmlElement* exprStmt = blockElement->FirstChildElement( "expr_stmt" );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, exprStmt, MessageType::MSG_TYPE_ERROR, "Missing expr_stmt block for reflected class %s.", className );

						TiXmlElement* expr = exprStmt->FirstChildElement( "expr" );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, expr, MessageType::MSG_TYPE_ERROR, "Missing expr block for reflected class %s.", className );

						TiXmlElement* literalElement = expr->FirstChildElement( "literal" );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, literalElement, MessageType::MSG_TYPE_ERROR, "Missing literal block for reflected class %s.", className );
						FEEDBACK_CHECK_RETURN_VALUE( false, context, literalElement->GetText(), MessageType::MSG_TYPE_ERROR, "Missing class file string for reflected class %s.", className );

						const char* headerFile = literalElement->GetText();
						reflectedType->SetFile( headerFile, context );
					} // End if label == CLASS_FILE
				} // End if label exists
			} // End if element is block

			blockElement = blockElement->NextSiblingElement();
		} // End while

		accessModifier = accessModifier->NextSiblingElement();
	} // End while

	return true;
}// End ReadClass()

bool CodeParser::ReadParents( TiXmlElement* superElement, FeedbackContext& context, const char*& parentOut )
{
	parentOut = nullptr;

	if( superElement != nullptr )
	{
		TiXmlElement* specifier = superElement->FirstChildElement( );
		FEEDBACK_CHECK_RETURN_VALUE( false, context, specifier, MessageType::MSG_TYPE_ERROR, "Expected specifier tag inside super block, found nothing instead." );
		FEEDBACK_CHECK_RETURN_VALUE( false, context, strcmp( specifier->Value(), "specifier" ) == 0, MessageType::MSG_TYPE_ERROR, "Expected specifier tag inside super block, found %s instead.", specifier->Value() );

		TiXmlElement* superName = nullptr;

		while( specifier != nullptr )
		{
			FEEDBACK_CHECK_RETURN_VALUE( false, context, strcmp( specifier->Value( ), "specifier" ) == 0, MessageType::MSG_TYPE_ERROR, "Expected specifier tag inside super block, found %s instead.", specifier->Value( ) );

			superName = specifier->NextSiblingElement();
			FEEDBACK_CHECK_RETURN_VALUE( false, context, superName, MessageType::MSG_TYPE_ERROR, "Expected name tag inside super block, found nothing instead." );
			FEEDBACK_CHECK_RETURN_VALUE( false, context, strcmp( superName->Value( ), "name" ) == 0, MessageType::MSG_TYPE_ERROR, "Expected name tag inside super block, found %s instead.", superName->Value( ) );

			if( strcmp( specifier->GetText( ), "public" ) == 0 && this->data->FindType( superName->GetText( ) ) != nullptr )
			{
				FEEDBACK_CHECK_RETURN_VALUE( false, context, parentOut == nullptr, MessageType::MSG_TYPE_ERROR, "Only one direct parent may be a reflected class." );
				parentOut = superName->GetText();
				FEEDBACK_CHECK_RETURN_VALUE( false, context, parentOut != nullptr, MessageType::MSG_TYPE_ERROR, "Expected name string inside of name block." );
			}

			specifier = superName->NextSiblingElement();
		}
	}

	return true;
}

void CodeParser::Finish()
{
	// Do nothing
}