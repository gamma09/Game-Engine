#include <stdarg.h>
#include <GameAssert.h>
#include <File.h>
#include "DataWriter.h"
#include "ReflectionData.h"

static void Write_String( FileHandle fh, bool newLine, const char* format, ... )
{
	char lengthTest;
	va_list args[2];
	va_start( args[0], format );
	va_copy( args[1], args[0] );

	int lengthRequired = vsnprintf( &lengthTest, 1, format, args[0] );
	va_end( args[0] );

	char* str = new char[lengthRequired + 1];
	GameVerify( lengthRequired == vsnprintf( str, lengthRequired + 1, format, args[1] ) );
	va_end( args[1] );

	GameVerify( FileError::FILE_SUCCESS == File::write( fh, str, strlen( str ) ) );

	if( newLine )
	{
		GameVerify( FileError::FILE_SUCCESS == File::write( fh, "\r\n", 2 ) );
	}
}

static void Write_Includes( FileHandle fh, ReflectionData* data )
{
	Write_String( fh, true, "#include <Class.h>" );
	Write_String( fh, true, "#include <Variable.h>" );

	for( const ReflectedType& type : data->GetTypes() )
	{
		Write_String( fh, true, "#include \"%s\"", type.GetFile() );
	}
}

static void Write_Magic_Offset_Incantation( FileHandle fh )
{
	Write_String( fh, true, "template<typename T, typename U>" );
	Write_String( fh, true, "constexpr size_t offsetOf( U T::*member )" );
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
	Write_String( fh, true, "private:" );

	for( const ReflectedType& type : data->GetTypes() )
	{
		Write_String( fh, true, "	static Class<%s>* %sInstance = nullptr;", type.GetName(), type.GetName() );
	}

	Write_String( fh, true, "public:" );

	for( const ReflectedType& type : data->GetTypes() )
	{
		
		Write_String( fh, true, "	template<>" );
		Write_String( fh, true, "	static const Class<%s>* LoadClass<%s>()", type.GetName(), type.GetName() );
		Write_String( fh, true, "	{" );
		Write_String( fh, true, "		if( %sInstance == nullptr )", type.GetName() );
		Write_String( fh, true, "		{" );
		Write_String( fh, true, "			%sInstance = new Class<%s>( \"%s\", LoadClass<%s>() );", type.GetName(), type.GetName(), type.GetName(), type.GetParentType() );

		for( const ReflectedVariable& var : type.GetVariables() )
		{
			Write_String( fh, true, "			%sInstance->variableHead = new Variable( \"%s\", *LoadClass<%s>(), offsetOf( &%s::%s ), %sInstance->variableHead );", type.GetName(), var.GetName(), var.GetTypeName(), type.GetName(), var.GetName(), type.GetName() );
		}

		Write_String( fh, true, "		}" );
		Write_String( fh, true, "		return %sInstance;", type.GetName() );
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
		Write_String( fh, true, "template<> const Class<%s>* Load_Class<%s>()", type.GetName(), type.GetName() );
		Write_String( fh, true, "{" );
		Write_String( fh, true, "	return ClassLoader::LoadClass<%s>();", type.GetName() );
		Write_String( fh, true, "}" );
	}
}


void Write_Reflection_Data( const char* outputFile, ReflectionData* data )
{
	FileHandle fh;
	GameVerify( FileError::FILE_SUCCESS == File::open( fh, outputFile, FileMode::FILE_WRITE ) );

	Write_Includes( fh, data );
	Write_Magic_Offset_Incantation( fh );
	Write_Class_Loader( fh, data );
	Write_Load_Class_Global_Functions( fh, data );

	GameVerify( FileError::FILE_SUCCESS == File::flush( fh ) );
	GameVerify( FileError::FILE_SUCCESS == File::close( fh ) );
}