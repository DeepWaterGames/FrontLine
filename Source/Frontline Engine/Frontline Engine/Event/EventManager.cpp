#include "EventManager.h"

static EventManager* eventManager = FL_NEW EventManager();

// --------------------------------------------------------------------------
// Function:		EventManager::Get
// Purpose:			Static method to get the singleton event manager.
// Parameters:		None
// --------------------------------------------------------------------------
EventManager* EventManager::Get(void)
{
	return eventManager;
}

// --------------------------------------------------------------------------
// Function:		EventManager::VAddListener
// Purpose:			Add a new event listener.
// Parameters:		Delegate function, event type to listen to
// --------------------------------------------------------------------------
bool EventManager::VAddListener(const EventListenerDelegate& eventDelegate, const EventType& type) {
	auto it = m_eventListeners.find(type);
	if (it == m_eventListeners.end())
	{
		EventListenerList ell;
		ell.push_back(eventDelegate);
		m_eventListeners[type] = ell;
	}
	else
	{
		for (auto iter = it->second.begin(); iter != it->second.end(); iter++) {
			if (eventDelegate == (*iter)) {
				return false;
			}
		}
		m_eventListeners[type].push_back(eventDelegate);
	}
	return true;
}

// --------------------------------------------------------------------------
// Function:		EventManager::VRemoveListener
// Purpose:			Remove an event listener
// Parameters:		Delegate to be removed, event type of delegate
// --------------------------------------------------------------------------
bool EventManager::VRemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type) {
	//basically 
	//auto it = m_eventListeners.find(type);
	//m_eventListeners[type].erase(it);
	bool success = false;
	auto it = m_eventListeners.find(type);
	if (it != m_eventListeners.end()) {
		for (auto iter = it->second.begin(); iter != it->second.end(); iter++) {
			if (eventDelegate == (*iter)) {
				m_eventListeners[type].erase(iter);
				success = true;
				break;
			}
		}
	}
	return success;
}

// --------------------------------------------------------------------------
// Function:		EventManager::VTriggerEvent
// Purpose:			Fire event with immediate callbacks - VQueueEvent usually better
// Parameters:		Event to trigger
// --------------------------------------------------------------------------
bool EventManager::VTriggerEvent(const IEventDataPtr& pEvent) const {
	bool processed = false;
	auto it = m_eventListeners.find(pEvent->VGetEventType());
	if (it != m_eventListeners.end()) {
		for (auto iter = it->second.begin(); iter != it->second.end(); iter++){
			EventListenerDelegate listener = (*iter);
			listener(pEvent);
			processed = true;
		}
	}
	return processed;
}

// --------------------------------------------------------------------------
// Function:		EventManager::VQueueEvent
// Purpose:			Add an event to the queue for processing.
// Parameters:		Event to queue
// --------------------------------------------------------------------------
bool EventManager::VQueueEvent(const IEventDataPtr& pEvent)
{
	auto findIt = m_eventListeners.find(pEvent->VGetEventType());
	if (findIt != m_eventListeners.end())
	{
		m_queues[m_activeQueue].push_back(pEvent);
		return true;
	}
	return false;
}

// --------------------------------------------------------------------------
// Function:		EventManager::VAbortEvent
// Purpose:			Abort a single event or all events of a specific type
// Parameters:		Type to abort, should delete all of type
// --------------------------------------------------------------------------
bool EventManager::VAbortEvent(const EventType& inType, bool allOfType) {
	bool success = false;
	//check to see if the event has delegates
	auto it = m_eventListeners.find(inType);
	if (it != m_eventListeners.end()) {
		//if so, iterate through the queue deleting the
		//events that match inType
		auto it = m_queues[m_activeQueue].begin();
		while (it != m_queues[m_activeQueue].end()) {
			auto thisIt = it;
			it++;
			if ((*thisIt)->VGetEventType() == inType) {
				m_queues[m_activeQueue].erase(thisIt);
				success = true;
				if (!allOfType) break;
			}
		}
	}
	return success;
}

// --------------------------------------------------------------------------
// Function:		EventManager::VUpdate
// Purpose:			Process the events in queue
// Parameters:		Maximum time to run before leaving the remaining events for the next call
// --------------------------------------------------------------------------
bool EventManager::VUpdate(unsigned long maxMillis) {
	while (!m_queues[m_activeQueue].empty()) {
		//get front of queue
		IEventDataPtr pEvent = m_queues[m_activeQueue].front();
		//delete or "pop" the front of the queue 
		m_queues[m_activeQueue].pop_front();
		auto it = m_eventListeners.find(pEvent->VGetEventType());
		if (it != m_eventListeners.end()) {
			for (auto iter = it->second.begin(); iter != it->second.end(); iter++) {
				EventListenerDelegate listener = (*iter);
				listener(pEvent);
			}
		}
	}
	return true;
}
