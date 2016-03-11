#include <GameAssert.h>
#include "AppInfo.h"

static bool IsPowerOfTwo( unsigned int x )
{
	if( x == 0 ) return false;

	while( x & 1 == 0 )
	{
		x = x << 1;
	}


	return x == 1;
}

AppInfo::AppInfo( const char* inTitle, unsigned int inSamples )
	: width( 0 ),
	height( 0 ),
	refreshRate( 30 ),
	samples( inSamples ),
	fullscreen( false ),
	vsync( false ),
	cursor( false ),
	fullscreenWindowed( false )
{
	GameAssert( inTitle != nullptr );
	GameAssert( strlen( inTitle ) < 128 );
	strcpy_s( this->title, inTitle );

	// We only allow 1, 2, 4, 8, 16, etc.
	GameAssert( IsPowerOfTwo( inSamples ) );
}

AppInfo::AppInfo( const AppInfo& info )
	: width( info.width ),
	height( info.height ),
	refreshRate( info.refreshRate ),
	samples( info.samples ),
	fullscreen( info.fullscreen ),
	vsync( info.vsync ),
	cursor( info.cursor ),
	fullscreenWindowed( info.fullscreenWindowed )
{
	strcpy_s( this->title, info.title );
}

AppInfo& AppInfo::operator=( const AppInfo& info )
{
	memcpy( this, &info, sizeof( AppInfo ) );
}

AppInfo::~AppInfo()
{
	// Do nothing
}

void AppInfo::SetSamples( unsigned int inSamples )
{
	GameAssert( IsPowerOfTwo( inSamples ) );
	this->samples = inSamples;
}

void AppInfo::DecreaseSampleCount()
{
	GameAssert( this->samples != 1 );
	this->samples = this->samples >> 1;
}
