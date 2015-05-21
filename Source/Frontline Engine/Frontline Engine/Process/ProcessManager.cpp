#include "ProcessManager.h"

//pushes a process to be run in the processList
void ProcessManager::AttachProcess(StrongProcessPtr pProcess) {
	m_processList.push_front(pProcess);
}
//deletes all processes from processList;
void ProcessManager::ClearAllProcesses() {
	m_processList.clear();
}
void ProcessManager::AbortAllProcesses(bool immediate)
{
	auto it = m_processList.begin();
	while (it != m_processList.end()) {
		ProcessList::iterator tempIt = it;
		++it;

		StrongProcessPtr pProcess = *tempIt;
		if (pProcess->IsAlive()) {
			pProcess->SetState(PROCESS_ABORTED);
			if (immediate) {
				pProcess->VOnExit(PROCESS_ABORTED);
				m_processList.erase(tempIt);
				auto findIt = pProcess->m_childProcessMap.find(PROCESS_ABORTED);
				if(findIt != pProcess->m_childProcessMap.end()) {
					for(auto iter = findIt -> second.begin(); iter != findIt -> second.end(); iter++) {
						AttachProcess(*iter);
					}
				}
			}
		}
	}
}
//runs through all processes and takes appropriate procedures according to
//the state
unsigned int ProcessManager::UpdateProcesses(unsigned long deltaMs)
{
	unsigned short int successCount = 0;
	unsigned short int failCount = 0;

	ProcessList::iterator it = m_processList.begin();
	while (it != m_processList.end())
	{
		// grab the next process
		StrongProcessPtr pCurrProcess = (*it);

		// save the iterator and increment the old one in case we need to remove this process from the list
		ProcessList::iterator thisIt = it;
		++it;

		// process is uninitialized, so initialize it
		if (pCurrProcess->currentProcessState == PROCESS_UNINITIALIZED)
			pCurrProcess->VOnInit();

		// give the process an update tick if it's running
		if (pCurrProcess->currentProcessState == PROCESS_RUNNING)
			pCurrProcess->VOnUpdate(deltaMs);

		// check to see if the process is dead
		if (pCurrProcess->IsDead())
		{
			// run the appropriate exit function
			pCurrProcess->VOnExit(pCurrProcess->currentProcessState);
			auto findIt = pCurrProcess->m_childProcessMap.find(pCurrProcess->currentProcessState);
			if(findIt != pCurrProcess->m_childProcessMap.end()) {
				for(auto iter = findIt -> second.begin(); iter != findIt -> second.end(); iter++) {
					IGame::gp_game->mp_log->logOther("PROCESS", "Parent Process Finished with Success: Now Attaching Child Process!");
					AttachProcess(*iter);
				}
			}
			m_processList.erase(thisIt);
		}
	}

	return ((successCount << 16) | failCount);
}