#pragma once

struct AppInfo
{
	char title[128];
	int windowWidth;
	int windowHeight;
	int majorVersion;
	int minorVersion;
	int samples;
#pragma warning( disable : 4201 )
	union
	{
		struct
		{
			unsigned int fullscreen : 1;
			unsigned int vsync : 1;
			unsigned int cursor : 1;
			unsigned int stereo : 1;
			unsigned int debug : 1;
		};

		unsigned int all;
	} flags;
};