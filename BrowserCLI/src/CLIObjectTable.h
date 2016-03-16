#pragma once

#ifndef BROWSER_CLI_INTERNAL
#error "CLIObjectTree.h may not be included outside of the BrowserCLI package."
#endif

using namespace System;

namespace CLI
{

	struct CLIObjectValue;

	class CLIObjectTable
	{
	public:
		CLIObjectTable( unsigned int tableSize );

		~CLIObjectTable();

		void Add( const void* ptr, Object^ obj );
		Object^ operator[] ( const void* ptr );
		Object^ Remove( const void* ptr );

	private:
		CLIObjectValue** table;
		unsigned int tableSize;

	private:
		CLIObjectTable( const CLIObjectTable& dict ) = delete;
		CLIObjectTable& operator=( const CLIObjectTable& dict ) = delete;
	};
}