#include "LockFramerateProcess.h"
#include <Windows.h>

LockFramerateProcess::LockFramerateProcess(int fps)
{
	delayMS = 1000 / fps;
}


LockFramerateProcess::~LockFramerateProcess()
{
}

void LockFramerateProcess::VOnUpdate(unsigned long deltaMs) {
	if (delayMS - deltaMs > 0 && deltaMs < delayMS) {
		Sleep(delayMS - deltaMs);
	}
}