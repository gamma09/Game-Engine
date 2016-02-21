#include <string>
#include <GameAssert.h>
#include <File.h>
#include "DataWriter.h"
#include "ReflectionData.h"

using namespace std;

static void Write_String( FileHandle fh, bool newLine, const string& str )
{
	GameVerify( FileError::FILE_SUCCESS == File::write( fh, str.c_str(), str.length() ) );

	if( newLine )
	{
		GameVerify( FileError::FILE_SUCCESS == File::write( fh, "\r\n", 2 ) );
	}
}

static void Write_Includes( FileHandle fh, ReflectionData* data )
{
	Write_String( fh, true, "#include <Class.h>" );
	Write_String( fh, true, "#include <Variable.h>" );
	Write_String( fh, true, "#include <Writeable.h>" );
	Write_String( fh, true, "#include <Readable.h>" );

	for( const ReflectedType& type : data->GetTypes() )
	{
		if( !type.IsPrimitive() )
		{
			Write_String( fh, true, string( "#include \"" ) + type.GetFile() + "\"" );
		}
	}
}

static void Write_Magic_Offset_Incantation( FileHandle fh )
{
	Write_String( fh, true, "template<typename T, typename U>" );
	Write_String( fh, true, "size_t offsetOf( U T::*member )" );
	Write_String( fh, true, "{" );
	Write_String( fh, true, "	return (const char*)&((const T*)nullptr->*member) - (const char*)nullptr;" );
	Write_String( fh, true, "}" );
}

static void Write_Class_Loader( FileHandle fh, ReflectionData* data )
{
	Write_String( fh, true, "class ClassLoader" );
	Write_String( fh, true, "{" );
	Write_String( fh, true, "public:" );
	Write_String( fh, true, "	template<typename T>" );
	Write_String( fh, true, "	static const Class<T>* LoadClass()" );
	Write_String( fh, true, "	{" );
	Write_String( fh, true, "		return nullptr;" );
	Write_String( fh, true, "	}" );

	for( const ReflectedType& type : data->GetTypes() )
	{
		
		Write_String( fh, true, "	template<>" );
		Write_String( fh, true, string("	static const Class<") + type.GetName() + ">* LoadClass<" + type.GetName() + ">()" );
		Write_String( fh, true, "	{" );
		Write_String( fh, true, string( "		static Class<" ) + type.GetName() + ">* Instance = nullptr;" );
		Write_String( fh, true, "		if( Instance == nullptr )" );
		Write_String( fh, true, "		{" );
		if( type.GetParentType() )
		{
			Write_String( fh, true, string( "			Instance = new Class<") + type.GetName() + ">( \"" + type.GetName() + "\", LoadClass<" + type.GetParentType() + ">() );" );
		}
		else
		{
			Write_String( fh, true, string( "			Instance = new Class<") + type.GetName() + ">( \"" + type.GetName() + "\", nullptr);" );
		}

		for( const ReflectedVariable& var : type.GetVariables() )
		{
			Write_String( fh, true, string("			Instance->variableHead = new Variable( \"") + var.GetName() + "\", *LoadClass<" + var.GetTypeName() + ">(), offsetOf( &" + type.GetName() + "::" + var.GetName() + " ), " + type.GetName() + "Instance->variableHead );" );
		}

		Write_String( fh, true, "		}" );
		Write_String( fh, true, "		return Instance;" );
		Write_String( fh, true, "	}" );
	}

	Write_String( fh, true, "};" );
}

static void Write_Load_Class_Global_Functions( FileHandle fh, ReflectionData* data )
{
	Write_String( fh, true, "template< typename T >" );
	Write_String( fh, true, "const Class<T>* Load_Class()" );
	Write_String( fh, true, "{" );
	Write_String( fh, true, "	return nullptr;" );
	Write_String( fh, true, "}" );

	for( const ReflectedType& type : data->GetTypes() )
	{
		Write_String( fh, true, string("template<> const Class<") + type.GetName() + ">* Load_Class<" + type.GetName() + ">()");
		Write_String( fh, true, "{" );
		Write_String( fh, true, string("	return ClassLoader::LoadClass<") + type.GetName() + ">();" );
		Write_String( fh, true, "}" );
	}
}

static void Write_Serialize_Functions( FileHandle fh, ReflectionData* data )
{
	data;

	Write_String( fh, true, "template< typename T >" );
	Write_String( fh, true, "void SerializeClass( T*, Writeable& )  { }" );

	// TODO add class-specific serialization functions
}

static void Write_Deserialize_Functions( FileHandle fh, ReflectionData* data )
{
	data;

	Write_String( fh, true, "template< typename T >" );
	Write_String( fh, true, "void DeserializeClass( T*, Readable& )  { }" );

	// TODO add class-specific deserialization functions
}


void Write_Reflection_Data( const char* outputFile, ReflectionData* data )
{
	FileHandle fh;
	GameCheckFatal( FileError::FILE_SUCCESS == File::open( fh, outputFile, FileMode::FILE_WRITE ), "Could not open output file for writing." );

	Write_Includes( fh, data );
	Write_Magic_Offset_Incantation( fh );
	Write_Class_Loader( fh, data );
	Write_Load_Class_Global_Functions( fh, data );
	Write_Serialize_Functions( fh, data );
	Write_Deserialize_Functions( fh, data );

	GameCheckFatal( FileError::FILE_SUCCESS == File::flush( fh ), "Could not flush output file." );
	GameCheckFatal( FileError::FILE_SUCCESS == File::close( fh ), "Could not close output file." );
}