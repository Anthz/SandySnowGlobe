#include "Timer.h"


Timer::Timer()
{
	time = 0;

	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTime);
}

Timer::~Timer()
{
}

void Timer::Update()
{
	INT64 currentTime;
	float diff, a, b;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	a = (float)(currentTime - prevTime);
	b = (1.0f / (float)freq);
	diff = a * b;
	time = diff;
	prevTime = currentTime;
}
