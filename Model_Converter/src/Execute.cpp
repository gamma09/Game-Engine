#include "Execute.h"

#include <Windows.h>

int Executor::Execute( const char* executable, const char* workingDir, const char* params )
{
	SHELLEXECUTEINFO info;
	info.cbSize = sizeof( SHELLEXECUTEINFO );
	info.fMask = SEE_MASK_NOCLOSEPROCESS;
	info.hwnd = NULL;
	info.lpVerb = "open";
	info.lpFile = executable;
	info.lpParameters = params;
	info.lpDirectory = workingDir;
	info.nShow = SW_SHOWNORMAL;

	ShellExecuteEx( &info );

	WaitForSingleObject( info.hProcess, INFINITE );

	DWORD exitCode = 0;
	GetExitCodeProcess( info.hProcess, &exitCode );
	return exitCode;
}