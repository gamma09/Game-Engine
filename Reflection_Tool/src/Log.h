#pragma once

#include <fstream>

void InitLogStream( const char* file );

extern std::ofstream logStream;
#define LOG( a ) logStream << (a) << std::endl
