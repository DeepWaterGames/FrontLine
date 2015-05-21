#include "Process.h"

Process::Process() {
	currentProcessState = PROCESS_UNINITIALIZED;
}
Process::~Process() {
	m_childProcessMap.clear();
}

// --------------------------------------------------------------------------
// Function:		Process::AttachChild
// Purpose:			Attach a new child process
// Parameters:		The child's completion state, a shared pointer to the new child
// --------------------------------------------------------------------------
void Process::AttachChild(ProcessState state, StrongProcessPtr pChild) {
	m_childProcessMap[state].push_back(pChild);
}

// --------------------------------------------------------------------------
// Function:		Process::RemoveChild
// Purpose:			Remove a specific child process
// Parameters:		The completion state of the child, a shared pointer to the child
// --------------------------------------------------------------------------
void Process::RemoveChild(ProcessState state, StrongProcessPtr pChild) {
	auto findIt = m_childProcessMap.find(state);
	if(findIt != m_childProcessMap.end()) {
		for(auto iter = findIt -> second.begin(); iter != findIt -> second.end();) {
			if(pChild == (*iter)) {
				m_childProcessMap[state].erase(iter);
				break;
			}
			else {
				iter++;
			}
		}
	}
}

// --------------------------------------------------------------------------
// Function:		Process::RemoveChild
// Purpose:			Remove all child processes with a specific completion state
// Parameters:		The completion state to clear
// --------------------------------------------------------------------------
void Process::RemoveChild(ProcessState state) {
	m_childProcessMap[state].clear();
}