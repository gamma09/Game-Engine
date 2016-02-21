#pragma once

#include <ostream>

// This function shall not be called by more than one thread at a time.
class Exec
{
public:
	static unsigned long Execute( const char* commandLine, const char* stdinStr, char** stdoutOutput, char** stderrOutput, std::ostream* logStream = nullptr );
};