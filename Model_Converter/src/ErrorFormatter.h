#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdio>



inline void Print_Error_Message( char* msg )
{
	SetConsoleTextAttribute( GetStdHandle( STD_ERROR_HANDLE ), FOREGROUND_RED );
	fprintf( stderr, msg );
	fprintf( stderr, "\n" );
}