#pragma once

const unsigned int FLAG_VSYNC      = 0x1;
const unsigned int FLAG_FULLSCREEN = 0x2;
const unsigned int FLAG_CURSOR     = 0x4;
const unsigned int FLAG_STEREO     = 0x8;
const unsigned int FLAG_DEBUG      = 0x16;
const unsigned int FLAG_ALL        = FLAG_FULLSCREEN | FLAG_VSYNC | FLAG_CURSOR | FLAG_STEREO | FLAG_DEBUG;

struct AppInfo
{
	char title[128];
	int windowWidth;
	int windowHeight;
	int majorVersion;
	int minorVersion;
	int samples;
	unsigned int flags;
};