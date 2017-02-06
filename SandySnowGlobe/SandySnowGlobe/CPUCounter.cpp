#include "CPUCounter.h"

CPUCounter::CPUCounter()
{
	prevTime = timeGetTime();
	CPU = 0;

	PdhOpenQuery(NULL, NULL, &cpuQuery);
	PdhAddCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
	PdhCollectQueryData(cpuQuery);
}

CPUCounter::~CPUCounter()
{
}

void CPUCounter::Update()
{
	if(timeGetTime() >= (prevTime + 1000))	//if 1 second has passed, query for CPU usage
	{
		PDH_FMT_COUNTERVALUE counter;
		PdhCollectQueryData(cpuQuery);
		PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counter);
		CPU = counter.doubleValue;
		prevTime = timeGetTime();
	}
}

