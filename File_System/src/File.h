
#ifndef FILE_H
#define FILE_H

#include <windows.h> // win32

// Make the assumption of c-char strings, not UNICODE
// 32 bit files, not supporting 64 bits


enum FileMode
{
	FILE_READ = 0xA0000000,
	FILE_WRITE,
	FILE_READ_WRITE
};

enum FileSeek
{
	FILE_SEEK_BEGIN = 0xC0000000,
	FILE_SEEK_CURRENT,
	FILE_SEEK_END
};

enum FileError
{
	FILE_SUCCESS = 0xB0000000,
	FILE_OPEN_FAIL,
	FILE_CLOSE_FAIL,
	FILE_WRITE_FAIL,
	FILE_READ_FAIL,
	FILE_SEEK_FAIL,
	FILE_TELL_FAIL,
	FILE_FLUSH_FAIL
};

typedef HANDLE FileHandle;

class File
{
public:

	static FileError open( FileHandle& fh, const char* fileName, FileMode mode );
	static FileError close( FileHandle fh );
	static FileError write( FileHandle fh, const void* buffer, size_t inSize );
	static FileError read( FileHandle fh, void* _buffer, size_t _size );
	static FileError seek( FileHandle fh, FileSeek seek, int offset );
	static FileError tell( FileHandle fh, unsigned long& offset );
	static FileError flush( FileHandle fh );
	static FileError size( FileHandle fh, unsigned long& size );
};

#endif