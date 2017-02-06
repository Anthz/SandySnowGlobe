#pragma once

#include <windows.h>

class Timer
{
public:
	Timer();
	~Timer();

	void Update();
	float Time() const { return time; }

private:
	INT64 freq, prevTime;
	float time;
};

