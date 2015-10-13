#include "WinFileWrapper.h"

HANDLE Create_File(const char* const filename, DWORD desiredAccess, DWORD sharedAccess, DWORD creationBehavior)
{
	return CreateFileA(filename, desiredAccess, sharedAccess, NULL, creationBehavior, FILE_ATTRIBUTE_NORMAL, NULL);
}

BOOL Close_File(const HANDLE hFile)
{
	return CloseHandle(hFile);
}

BOOL Flush_File(const HANDLE hFile)
{
	return FlushFileBuffers(hFile);
}

BOOL Is_Open(const HANDLE hFile)
{
	DWORD info;
	return GetHandleInformation(hFile, &info);
}

BOOL Read_Bytes(const HANDLE hFile, void* const buffer, DWORD bytesToRead)
{
	DWORD bytesRead;
	return ReadFile(hFile, buffer, bytesToRead, &bytesRead, NULL);
}

BOOL Seek_Position(const HANDLE hFile, int offset, DWORD moveMethod)
{
	DWORD position = SetFilePointer(hFile, offset, NULL, moveMethod);
	if (position == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
		return FALSE;
	else
		return TRUE;
}

BOOL Tell_Position(const HANDLE hFile, DWORD& position)
{
	position = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	if (position == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
		return FALSE;
	else
		return TRUE;
}

BOOL Write_Bytes(const HANDLE hFile, const void* const bytes, DWORD bytesToWrite)
{
	DWORD bytesWritten;
	return WriteFile(hFile, bytes, bytesToWrite, &bytesWritten, NULL);
}