#include "Log.h"

std::ofstream logStream;

void InitLogStream( const char* file )
{
	logStream = std::ofstream( file, std::ofstream::out | std::ofstream::trunc );
}