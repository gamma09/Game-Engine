#pragma once

enum TitleEnum
{
	Window_Title
};

enum WidthEnum
{
	Window_Width
};

enum HeightEnum
{
	Window_Height
};

enum RefreshRateEnum
{
	Refresh_Rate
};

enum FullscreenEnum
{
	Fullscreen
};

enum FullscreenWindowedEnum
{
	Fullscreen_Windowed
};

enum VsyncEnum
{
	Vsync
};

enum CursorEnum
{
	Cursor
};

enum SamplesEnum
{
	Samples
};


class AppInfo
{

public:
	AppInfo( const char* title, unsigned int samples );
	AppInfo( const AppInfo& info );
	AppInfo& operator=( const AppInfo& info );
	~AppInfo();

	inline const char*  operator[]( TitleEnum ) const { return title; }
	inline unsigned int operator[]( WidthEnum ) const { return width; }
	inline unsigned int operator[]( HeightEnum ) const { return height; }
	inline unsigned int operator[]( RefreshRateEnum ) const { return refreshRate; }
	inline unsigned int operator[]( SamplesEnum ) const { return samples; }
	inline bool         operator[]( FullscreenEnum ) const { return fullscreen; }
	inline bool         operator[]( FullscreenWindowedEnum ) const { return fullscreenWindowed; }
	inline bool         operator[]( VsyncEnum ) const { return vsync; }
	inline bool         operator[]( CursorEnum ) const { return cursor; }

	inline unsigned int& operator[]( WidthEnum ) { return width; }
	inline unsigned int& operator[]( HeightEnum ) { return height; }
	inline unsigned int& operator[]( RefreshRateEnum ) { return refreshRate; }
	inline bool&         operator[]( FullscreenEnum ) { return fullscreen; }
	inline bool&         operator[]( VsyncEnum ) { return vsync; }
	inline bool&         operator[]( FullscreenWindowedEnum ) { return fullscreenWindowed; }
	inline bool&         operator[]( CursorEnum ) { return cursor; }

	void DecreaseSampleCount();
	void SetSamples( unsigned int samples );


private:
	char title[128];
	unsigned int width;
	unsigned int height;
	unsigned int refreshRate;
	unsigned int samples;

	bool fullscreen;
	bool fullscreenWindowed;
	bool vsync;
	bool cursor;
};