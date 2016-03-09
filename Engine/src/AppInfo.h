#pragma once

struct AppInfo
{
	char title[128];
	int windowWidth;
	int windowHeight;
	int majorVersion;
	int minorVersion;
	int samples;

	bool fullscreen;
	bool vsync;
	bool cursor;
	bool stereo;
	bool debug;
};