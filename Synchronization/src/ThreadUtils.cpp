#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

#include "ThreadUtils.h"

#define MS_VC_EXCEPTION 0x406D1388

#pragma pack( push, 8 )
struct THREADNAME_INFO
{
	DWORD dwType;
	LPCSTR szName;
	DWORD dwThreadID;
	DWORD dwFlags;
};
#pragma pack( pop )

void ThreadUtils::SetThreadName( const char* threadName )
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = GetCurrentThreadId();
	info.dwFlags = 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof( info ) / sizeof( ULONG_PTR ), (ULONG_PTR*) &info );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}