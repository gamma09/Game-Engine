#include <windows.h>

HANDLE Create_File(const char* const filename, DWORD desiredAccess, DWORD sharedAccess, DWORD creationBehavior);

BOOL Close_File(const HANDLE hFile);

BOOL Flush_File(const HANDLE hFile);

BOOL Is_Open(const HANDLE hFile);

BOOL Read_Bytes(const HANDLE hFile, void* const buffer, DWORD bytesToRead);

BOOL Seek_Position(const HANDLE hFile, int offset, DWORD moveMethod);

BOOL Tell_Position(const HANDLE hFile, DWORD& position);

BOOL Write_Bytes(const HANDLE hFile, const void* const bytes, DWORD bytesToWrite);

