#include <cstdio>
#include <cstring>
#include <GameAssert.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "PackageHeader.h"


void process_package(char* version, char* name, char* outFile)
{
	PackageHeader header;
	header.totalSize = 0;
	header.numChunks = 0;
	strcpy_s(header.packageName, name);
	strcpy_s(header.versionString, version);

	char* buffer = new char[strlen(outFile) + 5];
	sprintf_s(buffer, strlen(outFile) + 5, "%s.brf", outFile);

	FILE* brf;
	fopen_s(&brf, buffer, "wb");

	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile("*.blo", &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		BOOL hasAnotherFile;

		do
		{
			FILE* file;
			fopen_s(&file, findData.cFileName, "rb");
			fseek(file, 0, SEEK_END);
			int size = ftell(file);
			fseek(file, 0, SEEK_SET);

			unsigned char* data = new unsigned char[size];
			fread(data, size, 1, file);
			fclose(file);

			header.numChunks++;
			header.totalSize += size;
			fwrite(data, size, 1, brf);
			fflush(brf);
			delete[] data;

			hasAnotherFile = FindNextFile(hFind, &findData);
		}
		while (hasAnotherFile);
	}

	fclose(brf);

	fopen_s(&brf, buffer, "rb");
	unsigned char* data = new unsigned char[header.totalSize];
	fread(data, header.totalSize, 1, brf);
	fclose(brf);

	DeleteFile(buffer);
	delete[] buffer;

	FILE* spu;
	fopen_s(&spu, outFile, "wb");
	fwrite(&header, sizeof(PackageHeader), 1, spu);
	fwrite(data, header.totalSize, 1, spu);
	fflush(spu);
	fclose(spu);

	delete[] data;
}




//----------------------------------------------------------------------------------------------------------------
// Command line stuff
//----------------------------------------------------------------------------------------------------------------

void print_usage(char* executable)
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "%s OUTPUT_FILE.spu <options>\n", executable);
	fprintf(stderr, "\n");
	fprintf(stderr, "Parameters\n");
	fprintf(stderr, "OUTPUT_FILE       The name of the package file that will be generated. This file must have the extension .spu\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Options\n");
	fprintf(stderr, "-v VERSION        The version of the engine which can use this package\n");
	fprintf(stderr, "-n PACKAGE_NAME   The name the package in the archive.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "LIU will archive all .blo files in the current working directory.\n");
}

bool ends_with(const char* str, const char* endStr)
{
	int strSize = strlen(str);
	int endStrSize = strlen(endStr);

	if (strlen(str) < strlen(endStr))
		return false;
	
	const char* endPtr = endStr + endStrSize;
	const char* strPtr = str + strSize;

	GameAssert(*endPtr == 0);
	GameAssert(*strPtr == 0);

	while (endPtr >= endStr)
	{
		if (*endPtr != *strPtr)
			return false;

		endPtr--;
		strPtr--;
	}

	return true;
}

void to_lower(char* str)
{
	for (char* pStr = str; *pStr != 0; pStr++)
	{
		if (*pStr >= 'A' && *pStr <= 'Z')
			*pStr = *pStr - 'A' + 'a';
	}
}

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
		to_lower(argv[i]);

	if (argc == 1)
	{
		print_usage(argv[0]);
		return 0;
	}

	if (argc != 6)
	{
		fprintf(stderr, "Incorrect number of arguments.\n");
		print_usage(argv[0]);
		return 1;
	}

	char* outFile = argv[1];

	if (!ends_with(outFile, ".spu"))
	{
		fprintf(stderr, "Output file name must end with extension .spu\n");
		return 2;
	}

	char* version = 0;
	char* name = 0;

	if (strcmp(argv[2], "-v") == 0)
		version = argv[3];
	else if (strcmp(argv[2], "-n") == 0)
		name = argv[3];
	else
	{
		fprintf(stderr, "Unrecognized option.\n");
		return 3;
	}

	if (strcmp(argv[4], "-v") == 0)
	{
		if (version != 0)
		{
			fprintf(stderr, "Option already specified.\n");
			return 4;
		}

		version = argv[5];
	}
	else if (strcmp(argv[4], "-n") == 0)
	{
		if (name != 0)
		{
			fprintf(stderr, "Option already specified.\n");
			return 4;
		}

		name = argv[5];
	}
	else
	{
		fprintf(stderr, "Unrecognized option.\n");
		return 3;
	}

	if (strlen(name) > 19)
	{
		fprintf(stderr, "Package name must be 19 characters long or less.\n");
		return 6;
	}

	if (strlen(version) > 19)
	{
		fprintf(stderr, "Package version must be 19 characters long or less.\n");
		return 6;
	}

	process_package(version, name, outFile);
	return 0;
}