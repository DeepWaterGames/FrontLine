#pragma once
#include "..\FrontlineCommon.h"
#include "Process.h"

class ProcessManager
{
	typedef std::list<StrongProcessPtr> ProcessList;

public:
	~ProcessManager(void) {}
	ProcessList m_processList; 
	unsigned int UpdateProcesses(unsigned long deltaMs);  // updates all attached processes
	void AttachProcess(StrongProcessPtr pProcess);  // attaches a process to the process mgr
	void AbortAllProcesses(bool immediate);
	unsigned int GetProcessCount() const { return m_processList.size(); }

private:
	void ClearAllProcesses();  // should only be called by the destructor
};
