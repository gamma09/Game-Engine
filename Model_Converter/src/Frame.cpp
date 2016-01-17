#include "Frame.h"

int Compare_Frames( const void* frameA, const void* frameB )
{
	return reinterpret_cast<const Frame*>( frameA )->frameTime - reinterpret_cast<const Frame*>( frameB )->frameTime;
}