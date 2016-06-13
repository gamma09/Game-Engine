#pragma once

template<size_t size>
void copy_string( char( &dst )[size], const char* src )
{
	char* endPtr = dst + size - 1;
	char* dstPtr;
	for( dstPtr = dst; dstPtr < endPtr && *src != 0; dstPtr++, src++ )
	{
		*dstPtr = *src;
	}

	*dstPtr = 0;
}