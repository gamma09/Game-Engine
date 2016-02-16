#include "Log.h"

#ifdef _DEBUG

std::ofstream logStream( "log.out", std::ofstream::out | std::ofstream::trunc );

#endif