#pragma once

#include <windows.h>
#include <mmsystem.h>
#include "psapi.h"

class RAMCounter
{
public:
	RAMCounter();
	~RAMCounter();

	void Update();

	int TotalUsedRAM() const { return totalUsedRAM / 1048576; }
	int UsedRAM() const { return usedRAM / 1048576; }
	int TotalRAM() const { return totalRAM / 1048576; }

private:
	MEMORYSTATUSEX memoryInfo;
	float totalRAM, usedRAM, totalUsedRAM;
	unsigned long prevTime;
};

