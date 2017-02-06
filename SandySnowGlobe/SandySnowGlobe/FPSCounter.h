#pragma once

#include <windows.h>
#include <mmsystem.h>

class FPSCounter
{
public:
	FPSCounter();
	~FPSCounter();

	void Update();
	unsigned int FPS() const { return fps; }

private:
	unsigned int fps, count;
	unsigned long prevTime;
};

