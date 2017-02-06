#pragma once

#include <Pdh.h>
#include <windows.h>
#include <mmsystem.h>

class CPUCounter
{
public:
	CPUCounter();
	~CPUCounter();

	void Update();
	unsigned int CPUUsage() const { return CPU; }

private:
	HQUERY cpuQuery;
	HCOUNTER cpuTotal;
	unsigned long prevTime;
	unsigned int CPU;
};

