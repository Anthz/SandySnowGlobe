#include "RAMCounter.h"


RAMCounter::RAMCounter()
{
	prevTime = timeGetTime();
	totalRAM = 0;
	usedRAM = 0;
	totalUsedRAM = 0;

	memoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memoryInfo);
	totalRAM = memoryInfo.ullTotalPhys;
}


RAMCounter::~RAMCounter()
{
}

void RAMCounter::Update()
{
	if(timeGetTime() >= (prevTime + 1000))	//if 1 second has passed, query for RAM usage
	{
		PROCESS_MEMORY_COUNTERS counter;
		GetProcessMemoryInfo(GetCurrentProcess(), &counter, sizeof(counter));
		usedRAM = counter.WorkingSetSize;
		totalUsedRAM = memoryInfo.ullTotalPhys - memoryInfo.ullAvailPhys;
		prevTime = timeGetTime();
	}
}
