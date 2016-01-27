#include <stdio.h>

void PrintUsage( const char* executable )
{
	fprintf( stderr, "Usage:\n" );
	fprintf( stderr, "  %s INT_DIR OUT_DIR    \n", executable );
	fprintf( stderr, "\n" );
	fprintf( stderr, "Parameters:\n" );
	fprintf( stderr, "  INT_DIR  The intermediate directory containing all of the preprocessed\n" );
	fprintf( stderr, "           source files. The tool will completely rebuild the reflection\n" );
	fprintf( stderr, "           database from those files.\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "  OUT_DIR  The output directory to write the reflection database to.\n" );
	fprintf( stderr, "           The directory's current contents will be deleted.\n" );
}

int main( int argc, char** argv )
{
	if ( argc != 3 )
	{
		PrintUsage( argv[0] );
		return 1;
	}
	
	
}