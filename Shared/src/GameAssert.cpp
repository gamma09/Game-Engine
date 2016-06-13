#include <stdlib.h>
#include <string>
#include "GameAssert.h"
#include "Logger.h"

#if defined(_DEBUG) || defined(DEBUG)

void Debugging::WarnIf( bool expr, const char* msg )
{
	if( expr )
	{
		Logger::Log_Warning( msg );

		int result = MessageBox( nullptr, msg, "WARNING", MB_ABORTRETRYIGNORE | MB_ICONWARNING | MB_DEFBUTTON1 | MB_TASKMODAL | MB_SETFOREGROUND );
		switch( result )
		{
			case IDABORT:
				exit( EXIT_FAILURE );
				break;

			case IDRETRY:
				_CrtDbgBreak();
				break;

			default:
				break;
		}
	}
}

#else

void Debugging::WarnIf( bool expr, const char* msg )
{
	if( expr )
	{
		Logger::Log_Warning( msg );

		int result = MessageBox( nullptr, (std::string( msg ) + "\n" + "Do you wish to ignore this warning and continue?").c_str(), "WARNING", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 | MB_TASKMODAL | MB_SETFOREGROUND );
		if( result == IDNO )
		{
			exit( EXIT_FAILURE );
		}
	}
}
#endif