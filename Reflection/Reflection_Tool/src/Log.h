#pragma once

#include <fstream>

#ifdef _DEBUG

extern std::ofstream logStream;
#define LOG( a ) logStream << a << std::endl

#else

#define LOG( a ) a

#endif