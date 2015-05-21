#include "Process.h"
#include "ProcessManager.h" 
#include <Windows.h>
#include <iostream>

typedef std::shared_ptr<ProcessManager>ProcessManagerPtr;

class ProcessTest : public Process {
public:
	ProcessTest(int id){ID = id;}
	~ProcessTest(){}

	void VOnInit() { 
		currentProcessState = PROCESS_RUNNING; 
		timer = 0;
		std::cout<<"initializing process!"<<std::endl;
	} 
	void VOnUpdate(unsigned long deltaMs){ 
		timer += deltaMs;
		std::cout<<"updating Process with VOnUpdate!"<<" "<<deltaMs<<" "<<timer<<" "<<ID<<std::endl;
		if(timer == 30) {
			currentProcessState = PROCESS_SUCCEDED;
		}
	};  
	void VOnExit(ProcessState exitState) {
		std::cout<<"successfuly exited the process with exit state: "<<exitState<<std::endl;
	}
private:
	int timer;
	int ID;
};

int main() {
	ProcessManagerPtr processManager(new ProcessManager());
	std::shared_ptr<ProcessTest>pProcessTest(new ProcessTest(1));
	std::shared_ptr<ProcessTest>pProcessTest2(new ProcessTest(2));
	std::shared_ptr<ProcessTest>pChildProcess(new ProcessTest(3));
	std::shared_ptr<ProcessTest>pChildProcess2(new ProcessTest(4));
	//attaching the parent process
	processManager -> AttachProcess(pProcessTest); 
	processManager -> AttachProcess(pProcessTest2); 
	//attaching the child process which will be automatically
	//attached to the manager as a parent process when the
	//original parent process finishes and gets deleted
	pProcessTest -> AttachChild(PROCESS_SUCCEDED,pChildProcess);
	pProcessTest2 -> AttachChild(PROCESS_SUCCEDED,pChildProcess2);
	while(processManager -> m_processList.size() != 0) {
		processManager -> UpdateProcesses(5);
	}
	MessageBox(NULL,"No Runtime Errors","INFO",MB_OK);
	return 0;
}
