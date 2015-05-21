#pragma once
#include <Process\Process.h>
#include <FrontlineCommon.h>

class LockFramerateProcess :
	public Process
{
	int delayMS;
	int accumMS;
public:
	virtual void VOnUpdate(unsigned long deltaMs) override;
	void SetFramerate(int fps);
	LockFramerateProcess(int fps);
	~LockFramerateProcess();
};