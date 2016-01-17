#pragma once

class Executor
{
public:
	static int Execute( const char* executable, const char* workingDir, const char* params );
};