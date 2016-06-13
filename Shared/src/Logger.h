#pragma once

#include "File.h"

class Logger
{
public:
	static void Start_Log( const char* logFile, bool bAsync );
	
	static void Log_Debug( const char* format, ... );
	static void Log_Info( const char* format, ... );
	static void Log_Warning( const char* format, ... );
	static void Log_Error( const char* format, ... );

	static void Stop_Log();

protected:
	virtual void Write_To_Log( char* str ) abstract;

	Logger( const FileHandle& logFile );
	virtual ~Logger();


	void WriteString( const char* str ) const;

private:
	FileHandle logFile;

	static Logger* logger;


private:

	Logger( const Logger& ) = delete;
	Logger& operator=( const Logger& ) = delete;


};