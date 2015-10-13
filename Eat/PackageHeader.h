#ifndef PACKAGE_HEADER_H
#define PACKAGE_HEADER_H

#define PACKAGE_VERSION_SIZE 20
#define PACKAGE_NAME_SIZE 20

struct PackageHeader
{
   char     packageName[PACKAGE_NAME_SIZE];
   char     versionString[PACKAGE_VERSION_SIZE];
   int      numChunks;
   int      totalSize;   // size of file (without package header)
};



#endif