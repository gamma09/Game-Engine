#include <GameAssert.h>
#include <stdio.h>

#include "File.h"
#include "WinFileWrapper.h"

FileError File::open(FileHandle &fh, const char * const fileName, FileMode mode)
{
	if (fileName == 0)
		return FILE_OPEN_FAIL;

	// Yes, these are temporaries, but the CreateFile operation will take a long time anyway, so what's 3 more operations?
	DWORD desiredAccess = 0;
	DWORD sharedAccess = 0;
	DWORD creationBehavior = 0;

	switch (mode)
	{
		case FILE_READ:
			// If we're just opening to read, allow others to open to read
			desiredAccess = GENERIC_READ;
			sharedAccess = FILE_SHARE_READ;
			creationBehavior = OPEN_EXISTING;
			break;

		case FILE_WRITE:
			// If we're opening to write, no one else is allowed to open this file
			desiredAccess = GENERIC_WRITE;
			sharedAccess = 0;
			creationBehavior = CREATE_ALWAYS;
			break;

		case FILE_READ_WRITE:
			// Since we're opening to write, no one else is allowed to open this file
			desiredAccess = GENERIC_READ | GENERIC_WRITE;
			sharedAccess = 0;
			creationBehavior = OPEN_ALWAYS;
			break;

		default:
			// Should never happen
			// Did you add an additional FileMode to the enum, but forget to add it to this switch statement?
			GameAssert(0);
	}


	fh = Create_File(fileName, desiredAccess, sharedAccess, creationBehavior);

	if (fh == INVALID_HANDLE_VALUE)
		return FILE_OPEN_FAIL;

	return FILE_SUCCESS;
}

FileError File::close(const FileHandle fh)
{
	if (!Is_Open(fh) || !Close_File(fh))
		return FILE_CLOSE_FAIL;
	else
		return FILE_SUCCESS;
}

FileError File::write(FileHandle fh, const void * const buffer, const size_t inSize)
{
	if (!Is_Open(fh) || !Write_Bytes(fh, buffer, inSize))
		return FILE_WRITE_FAIL;
	else
		return FILE_SUCCESS;
}

FileError File::read(FileHandle fh,  void * const buffer, const size_t inSize)
{
	if (!Is_Open(fh) || !Read_Bytes(fh, buffer, inSize))
		return FILE_READ_FAIL;
	else
		return FILE_SUCCESS;
}

// TODO switch to unsigned long after milestone 1
FileError File::seek(FileHandle fh, FileSeek seek, int offset)
{
	DWORD moveMethod = 0;
	switch (seek)
	{
		case FILE_SEEK_BEGIN:
			moveMethod = FILE_BEGIN;
			break;

		case FILE_SEEK_CURRENT:
			moveMethod = FILE_CURRENT;
			break;

		case FILE_SEEK_END:
			moveMethod = FILE_END;
			break;

		default:
			// Should never happen
			// Did you add an additional seek method without updating this switch statement?
			GameAssert(0);
	}

	if (!Is_Open(fh) || !Seek_Position(fh, offset, moveMethod))
		return FILE_SEEK_FAIL;
	else
		return FILE_SUCCESS;
}

// TODO switch to DWORD after milestone 1...
// why is it even an int to begin with? you can't have NEGATIVE file offsets...
FileError File::tell(FileHandle fh, int &offset)
{
	DWORD position;

	if (!Is_Open(fh) || !Tell_Position(fh, position)) {
		return FILE_TELL_FAIL;
	}else{
		offset = position;
		return FILE_SUCCESS;
	}
}

FileError File::flush(FileHandle fh)
{
	if (!Is_Open(fh) || !Flush_File(fh))
		return FILE_FLUSH_FAIL;
	else
		return FILE_SUCCESS;
}

FileError File::size(FileHandle fh, int& size)
{
	int currentLocation;
	FileError status = File::tell(fh, currentLocation);
	if (status != FILE_SUCCESS)
		return status;

	status = File::seek(fh, FILE_SEEK_END, 0);
	if (status == FILE_SUCCESS)
		status = File::tell(fh, size);

	File::seek(fh, FILE_SEEK_BEGIN, currentLocation);

	return status;
}