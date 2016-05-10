#include "DebugPrint.h"
#include <stdarg.h>
#include <stdio.h>

#ifdef DEBUG

void PrintDebugOutput( const char* format, ... )
{
	va_list ap;
	va_start( ap, format );

	vprintf( format, ap );

	va_end( ap );

	fflush( stdout );
}

void PrintPrefix( int levels )
{
	for( int i = 0; i < levels; i++ )
	{
		printf( "  " );
	}
}

#endif