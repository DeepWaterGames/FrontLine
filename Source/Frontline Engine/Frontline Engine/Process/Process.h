#pragma once
#include "..\FrontlineCommon.h"
#include <list>

#define PROCESS_UNINITIALIZED 0
#define PROCESS_REMOVED 1
#define PROCESS_PAUSED 2
#define PROCESS_SUCCEDED 3
#define PROCESS_FAILED 4
#define PROCESS_ABORTED 5
#define PROCESS_RUNNING 6
#define PROCESS_DECISION_TREE 1 << 31

class Process;
typedef std::shared_ptr<Process> StrongProcessPtr;
typedef std::list<StrongProcessPtr> StrongProcessPtrList;
typedef unsigned short ProcessState;

//the ints are the states so u can call certain child functions when u wanna
typedef std::map<int,StrongProcessPtrList>ChildProcessMap;

class Process {
	friend class ProcessManager;
	public:
		Process();
		virtual ~Process();
		ChildProcessMap m_childProcessMap;
		ProcessState currentProcessState;
	protected:
		//these functions should be overridden by the subclass as needed
		//StrongProcessPtr getProcess() {return std::make_shared<Process>(this);}
		virtual void VOnInit() { currentProcessState = PROCESS_RUNNING; } 
		virtual void VOnUpdate(unsigned long deltaMs) = 0;  
		virtual void VOnExit(ProcessState exitState) {}
	public:
		// Functions for ending the process.
		inline void Succeed() {currentProcessState = PROCESS_SUCCEDED;}
		inline void Fail() {currentProcessState = PROCESS_FAILED;}
	
		// pause
		inline void Pause() {currentProcessState = PROCESS_PAUSED;}
		inline void UnPause() {currentProcessState = PROCESS_RUNNING;}

		bool IsAlive() const { return (currentProcessState == PROCESS_RUNNING || currentProcessState == PROCESS_PAUSED); }
		bool IsDead() const { return !IsAlive(); }
		ProcessState GetState() { return currentProcessState; }

		// child functions
		void AttachChild(ProcessState state, StrongProcessPtr pChild);
		void RemoveChild(ProcessState state);  // releases ownership of the child
		void RemoveChild(ProcessState state, StrongProcessPtr pChild);  // releases ownership of the child
		
	private:
		void SetState(ProcessState newState) { currentProcessState = newState; }
};

