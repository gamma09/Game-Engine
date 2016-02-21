#include <GameAssert.h>
#include <tinyxml.h>
#include "CodeParser.h"
#include "ReflectionData.h"
#include "FeedbackContext.h"
#include "Log.h"

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
	FEEDBACK_CHECK_RETURN_XML( context, unit, MessageType::MSG_TYPE_ERROR, "No root element found in srcml.", doc );
	FEEDBACK_CHECK_RETURN_XML( context, strcmp( unit->Value( ), "unit" ) == 0, MessageType::MSG_TYPE_ERROR, (std::string("Unknown root element type in srcml: ") + unit->Value( )).c_str(), unit );

	ReadUnit( unit, context, "" );

	
}

bool CodeParser::ReadUnit( TiXmlElement* unit, FeedbackContext& context, const char* namespaceSoFar )
{
	TiXmlElement* unitElement = unit->FirstChildElement( );
	while( unitElement != nullptr )
	{
		if( strcmp( unitElement->Value( ), "namespace" ) == 0 )
		{
			FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, unitElement->FirstChildElement( "name" ), MessageType::MSG_TYPE_ERROR, "Anonymous namespace not allowed.", *unitElement );
			FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, unitElement->FirstChildElement( "name" )->GetText(), MessageType::MSG_TYPE_ERROR, "Anonymous namespace not allowed.", *unitElement );
			char* newNamespaceSoFar = new char[strlen( namespaceSoFar ) + 2 + strlen( unitElement->FirstChildElement( "name" )->GetText() ) + 1];
			sprintf( newNamespaceSoFar, "%s::%s", namespaceSoFar, unitElement->FirstChildElement( "name" )->GetText() );

			FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, unitElement->FirstChildElement( "block" ), MessageType::MSG_TYPE_ERROR, (std::string("Namespace ") + newNamespaceSoFar + " must have content block.").c_str(), *unitElement );
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

	if( classElement->FirstChildElement( "template" ) != nullptr )
	{
		return true;
	}

	TiXmlElement* nameElement = classElement->FirstChildElement( "name" );
	FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, nameElement, MessageType::MSG_TYPE_ERROR, "Unexpected end of class/struct tag.", *classElement );

	const char* className = nameElement->GetText();
	FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, className, MessageType::MSG_TYPE_ERROR, "Class name not found.", *classElement );

	const char* reflectedParent;
	if( !ReadParents( classElement->FirstChildElement( "super" ), context, reflectedParent, className ) ) return false;

	TiXmlElement* classBlock = classElement->FirstChildElement( "block" );
	FEEDBACK_CHECK_RETURN_VALUE_XML( true, context, classBlock, MessageType::MSG_TYPE_WARNING, (std::string("Class ") + className + " has no block tag.").c_str(), *classElement );

	ReflectedType* reflectedType = nullptr;

	TiXmlElement* accessModifier = classBlock->FirstChildElement();
	while( accessModifier != nullptr )
	{
		bool isPrivate = strcmp( accessModifier->Value(), "private" ) == 0;
		bool isPublic = strcmp( accessModifier->Value(), "public" ) == 0;
		bool isProtected = strcmp( accessModifier->Value(), "protected" ) == 0;
		FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, isPrivate || isPublic || isProtected, MessageType::MSG_TYPE_ERROR, (std::string("Unknown class storage modifier: ") + accessModifier->Value() + ".").c_str(), *classElement );

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
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, exprStmt, MessageType::MSG_TYPE_ERROR, (std::string("Missing expr_stmt block for reflected class ") + className + ".").c_str(), *classElement );

						TiXmlElement* expr = exprStmt->FirstChildElement( "expr" );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, expr, MessageType::MSG_TYPE_ERROR, (std::string( "Missing expr block for reflected class ") + className + ".").c_str(), *classElement );

						TiXmlElement* name = expr->FirstChildElement( "name" );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, name, MessageType::MSG_TYPE_ERROR, (std::string("Missing class name block for reflected class ") + className = ".").c_str(), *classElement );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, name->GetText(), MessageType::MSG_TYPE_ERROR, (std::string("Missing class name string for reflected class ") + className + ".").c_str(), *classElement );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, strcmp( name->GetText(), className ) == 0, MessageType::MSG_TYPE_ERROR, (std::string("Mismatch: Actual class name is ") + className + ", but the reflected class name is " + name->GetText() + ".").c_str(), *classElement);

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
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, reflectedType, MessageType::MSG_TYPE_ERROR, (std::string("Reflected variable declared before class reflection declaration in class ") + className + ".").c_str(), *classElement );

						TiXmlElement* declStmt = blockElement->FirstChildElement( "decl_stmt" );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, declStmt, MessageType::MSG_TYPE_ERROR, (std::string("Bad reflection variable declaration srcml block for class ") + className + ".").c_str(), *classElement );

						TiXmlElement* decl = declStmt->FirstChildElement( "decl" );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, decl, MessageType::MSG_TYPE_ERROR, ( std::string( "Bad reflection variable declaration srcml block for class ") + className + ".").c_str(), *classElement );

						TiXmlElement* type = decl->FirstChildElement( "type" );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, type, MessageType::MSG_TYPE_ERROR, ( std::string( "Bad reflection variable declaration srcml block for class ") + className + ".").c_str(), *classElement );

						TiXmlElement* name = type->FirstChildElement( "name" );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, name, MessageType::MSG_TYPE_ERROR, ( std::string( "Bad reflection variable declaration srcml block for class " ) + className + "." ).c_str( ), *classElement );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, name->GetText( ), MessageType::MSG_TYPE_ERROR, ( std::string( "Bad reflection variable declaration srcml block for class " ) + className + "." ).c_str( ), *classElement );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, this->data->FindType( name->GetText( ) ), MessageType::MSG_TYPE_ERROR, ( std::string( "Cannot declare a variable as reflected if its class type is not reflected itself. Variable type ") + name->GetText() + " in class " + className + ".").c_str(), *classElement );

						const char* variableTypename = name->GetText();

						TiXmlElement* valueNameElement = decl->FirstChildElement( "name" );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, valueNameElement, MessageType::MSG_TYPE_ERROR, ( std::string( "Bad reflection variable declaration srcml block for class " ) + className + "." ).c_str( ), *classElement );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, valueNameElement->GetText( ), MessageType::MSG_TYPE_ERROR, ( std::string( "Bad reflection variable declaration srcml block for class " ) + className + "." ).c_str( ), *classElement );

						const char* variableIdentifier = valueNameElement->GetText();

						reflectedType->AddVariable( variableIdentifier, variableTypename, context );
					} // End if label == VARIABLE_IS_REFLECTED
					else if( strcmp( labelElement->FirstChildElement( "name" )->GetText(), "CLASS_FILE" ) == 0 )
					{
						TiXmlElement* exprStmt = blockElement->FirstChildElement( "expr_stmt" );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, exprStmt, MessageType::MSG_TYPE_ERROR, ( std::string( "Missing expr_stmt block for reflected class " ) + className + "." ).c_str( ), *classElement );

						TiXmlElement* expr = exprStmt->FirstChildElement( "expr" );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, expr, MessageType::MSG_TYPE_ERROR, ( std::string( "Missing expr block for reflected class " ) + className + "." ).c_str( ), *classElement );

						TiXmlElement* literalElement = expr->FirstChildElement( "literal" );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, literalElement, MessageType::MSG_TYPE_ERROR, ( std::string( "Missing literal block for reflected class " ) + className + "." ).c_str( ), *classElement );
						FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, literalElement->GetText( ), MessageType::MSG_TYPE_ERROR, ( std::string( "Missing class file string for reflected class " ) + className + "." ).c_str( ), *classElement );

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

bool CodeParser::ReadParents( TiXmlElement* superElement, FeedbackContext& context, const char*& parentOut, const char* className )
{
	parentOut = nullptr;

	if( superElement != nullptr )
	{
		TiXmlElement* specifier = superElement->FirstChildElement( );
		FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, specifier, MessageType::MSG_TYPE_WARNING, "Expected specifier tag inside super block, found nothing instead.", *superElement );
		FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, strcmp( specifier->Value( ), "specifier" ) == 0, MessageType::MSG_TYPE_WARNING, ( std::string( "Expected specifier tag inside super block, found " ) + specifier->Value( ) + " instead." ).c_str( ), *superElement );

		TiXmlElement* superName = nullptr;

		while( specifier != nullptr )
		{
			FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, strcmp( specifier->Value( ), "specifier" ) == 0, MessageType::MSG_TYPE_ERROR, ( std::string( "Expected specifier tag inside super block, found " ) + specifier->Value( ) + " instead." ).c_str( ), *superElement );

			superName = specifier->NextSiblingElement();
			FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, superName, MessageType::MSG_TYPE_ERROR, "Expected name tag inside super block, found nothing instead.", *superElement );
			FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, strcmp( superName->Value( ), "name" ) == 0, MessageType::MSG_TYPE_ERROR, ( std::string( "Expected name tag inside super block, found " ) + superName->Value( ) + " instead." ).c_str( ), *superElement );
			FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, superName->GetText( ), MessageType::MSG_TYPE_ERROR, ( std::string( "No parent type string found for class " ) + className ).c_str( ), *superElement );

			if( strcmp( specifier->GetText( ), "public" ) == 0 && this->data->FindType( superName->GetText( ) ) != nullptr )
			{
				FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, parentOut == nullptr, MessageType::MSG_TYPE_ERROR, "Only one direct parent may be a reflected class.", *superElement );
				parentOut = superName->GetText();
				FEEDBACK_CHECK_RETURN_VALUE_XML( false, context, parentOut != nullptr, MessageType::MSG_TYPE_ERROR, "Expected name string inside of name block.", *superElement );
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