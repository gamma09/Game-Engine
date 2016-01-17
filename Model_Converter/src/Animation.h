#pragma once

#include <vector>
#include <string>
#include "Frame.h"

using namespace std;

class Animation
{
public:
	Animation( const char* name );
	Animation( const Animation& anim );
	Animation& operator=( const Animation& anim );
	~Animation();

	void Add_Frame( const Frame& frame );

	const char* Get_Name() const;
	const vector<Frame>& Get_Animation_Frames();

private:
	vector<Frame> frames;
	string name;
};