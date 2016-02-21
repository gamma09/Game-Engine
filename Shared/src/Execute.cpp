#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <stdlib.h>
#include <thread>

#include "Execute.h"
#include "GameAssert.h"



static SECURITY_ATTRIBUTES SetupSecurityAttributes( )
{
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof( SECURITY_ATTRIBUTES );
	saAttr.bInheritHandle = true;
	saAttr.lpSecurityDescriptor = nullptr;

	return saAttr;
}

struct PipeHandles
{
	HANDLE hRead;
	HANDLE hWrite;
};

static PipeHandles SetupPipe( SECURITY_ATTRIBUTES* saAttr, bool inheritRead, bool inheritWrite )
{
	PipeHandles handles;

	GameCheckFatal( CreatePipe( &handles.hRead, &handles.hWrite, saAttr, 0 ), "Could not create pipe for child process." );
	if( !inheritRead )
	{
		GameCheckFatal( SetHandleInformation( handles.hRead, HANDLE_FLAG_INHERIT, 0 ), "Could not set up pipe's read handle." );
	}

	if( !inheritWrite )
	{
		GameCheckFatal( SetHandleInformation( handles.hWrite, HANDLE_FLAG_INHERIT, 0 ), "Could not set up pipe's write handle." );
	}

	return handles;
}

static HANDLE CreateChildProcess( const char* commandLine, PipeHandles& childStdin, PipeHandles& childStderr, PipeHandles& childStdout )
{	
	STARTUPINFO* siStartInfo = (STARTUPINFO*) calloc( sizeof( STARTUPINFO ), 1 );
	PROCESS_INFORMATION* piProcInfo = (PROCESS_INFORMATION*) calloc( sizeof( PROCESS_INFORMATION ), 1 );

	siStartInfo->cb = sizeof( STARTUPINFO );
	siStartInfo->hStdInput = childStdin.hRead;
	siStartInfo->hStdError = childStderr.hWrite;
	siStartInfo->hStdOutput = childStdout.hWrite;
	siStartInfo->dwFlags |= STARTF_USESTDHANDLES;

	int length = strlen( commandLine ) + 1;
	char* copiedCommandLine = strncpy( new char[length], commandLine, length );


	GameCheckFatal( CreateProcess( NULL, copiedCommandLine, NULL, NULL, TRUE, 0, NULL, NULL, siStartInfo, piProcInfo ), "Could not create child process." );
	delete copiedCommandLine;

	CloseHandle( piProcInfo->hThread );
	CloseHandle( childStderr.hWrite );
	CloseHandle( childStdout.hWrite );
	CloseHandle( childStdin.hRead );
	HANDLE hProcess = piProcInfo->hProcess;

	free( siStartInfo );
	free( piProcInfo );

	return hProcess;
}

static void WriteToChild( HANDLE hWrite, const char* str )
{
	if( str )
	{
		GameCheckFatal( WriteFile( hWrite, str, strlen( str ), nullptr, nullptr ), "Could not write to child process's stdin." );
	}

	GameCheckFatal( CloseHandle( hWrite ), "Could not close the handle used to write to child process's stdin." );
}

#define BUFFER_SIZE 4096

static void ReadFromChild( HANDLE hRead, char** dest )
{
	std::string str;

	DWORD dwRead;
	// buf shall always be null-terminated, regardless of how many bytes ReadFile reads
	char* buf = (char*) calloc( BUFFER_SIZE + 1, sizeof( char ) );
	BOOL bSuccess = ReadFile( hRead, buf, BUFFER_SIZE, &dwRead, NULL );
	while( bSuccess && dwRead > 0 )
	{
		str += buf;

		bSuccess = ReadFile( hRead, buf, BUFFER_SIZE, &dwRead, NULL );
	}

	*dest = new char[str.length() + 1];
	strcpy( *dest, str.c_str() );
}

unsigned long Exec::Execute( const char* commandLine, const char* stdinStr, char** stdoutOutput, char** stderrOutput, std::ostream* logStream )
{
	SECURITY_ATTRIBUTES saAttr = SetupSecurityAttributes( );

	PipeHandles childStdin = SetupPipe( &saAttr, true, false );
	PipeHandles childStderr = SetupPipe( &saAttr, false, true );
	PipeHandles childStdout = SetupPipe( &saAttr, false, true );

	if( logStream )
	{
		*logStream << "Run: " << commandLine << std::endl;
	}

	HANDLE hChild = CreateChildProcess( commandLine, childStdin, childStderr, childStdout );
	WriteToChild( childStdin.hWrite, stdinStr );

	std::thread outThread( ReadFromChild, childStdout.hRead, stdoutOutput );
	std::thread errThread( ReadFromChild, childStderr.hRead, stderrOutput );

	outThread.join();
	errThread.join();
	
	GameCheckFatal( WAIT_FAILED != WaitForSingleObject( hChild, INFINITE ), "Could not call WaitForSingleObject() on the child process." );
	DWORD exitCode;
	GameCheckFatal( GetExitCodeProcess( hChild, &exitCode ), "Could not get the exit code of the child process." );

	CloseHandle( childStderr.hRead );
	CloseHandle( childStdout.hRead );

	CloseHandle( hChild );

	return exitCode;
}

