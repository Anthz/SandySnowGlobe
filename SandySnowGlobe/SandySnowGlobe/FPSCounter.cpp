#include "FPSCounter.h"


FPSCounter::FPSCounter()
{
	fps = 0;
	count = 0;
	prevTime = timeGetTime();
}


FPSCounter::~FPSCounter()
{
}

void FPSCounter::Update()
{
	count++;

	if(timeGetTime() >= (prevTime + 1000))	//if 1 second has passed, store fps value, otherwise continue incrementing
	{
		fps = count;
		count = 0;
		prevTime = timeGetTime();
	}
}
