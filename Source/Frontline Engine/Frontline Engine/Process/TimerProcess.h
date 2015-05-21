#pragma once
#include "..\FrontlineCommon.h"
#include "Process.h"
class TimerProcess :
	public Process
{
	friend class ProcessManager;
public:
	TimerProcess();
	int m_countdownMS;
protected:
	virtual void VOnUpdate(unsigned long deltaMs) override;
public:
	void SetTimer(int pi_n_countdownMS) { m_countdownMS = pi_n_countdownMS; }
};