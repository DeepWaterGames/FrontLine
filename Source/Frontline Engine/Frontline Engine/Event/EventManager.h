#pragma once
#include "../FrontlineCommon.h"
#include "../IGame.h"
#include <iostream>
#include <strstream>
#include <map>
#include <list>
#include "DELEGATE\FastDelegate.h"
#include <memory>

#define EVENTMANAGER_NUM_QUEUES 2

class IEventData;

typedef unsigned long EventType;
typedef std::shared_ptr<IEventData> IEventDataPtr; //pointer to class IEventData 
typedef fastdelegate::FastDelegate1<IEventDataPtr> EventListenerDelegate; //for a one parameter delegate

class IEventData {
public:
	virtual ~IEventData(void) {}
	virtual const EventType& VGetEventType(void) const = 0;
	virtual float GetTimeStamp(void) const = 0;
	virtual void VSerialize(std::ostrstream& out) const = 0;
	virtual void VDeserialize(std::istrstream& in) = 0;
	//virtual IEventDataPtr VCopy(void) const = 0;
	virtual const char* GetName(void) const = 0;
};
class BaseEventData : public IEventData {
	const float m_timeStamp;
public:
	explicit BaseEventData(const float timeStamp = 0.0f) : m_timeStamp(timeStamp) { }
	virtual const EventType& VGetEventType(void) const = 0;
	float GetTimeStamp(void) const { return m_timeStamp; }
	virtual void VSerialize(std::ostrstream &out) const	{ }
	virtual void VDeserialize(std::istrstream& in) { }
};
class EventManager {
	typedef std::list<EventListenerDelegate> EventListenerList; //may not be needed 
	typedef std::map<EventType, EventListenerList> EventListenerMap;
	typedef std::list<IEventDataPtr> EventQueue;

public:
	EventListenerMap m_eventListeners;
	EventQueue m_queues[EVENTMANAGER_NUM_QUEUES];
	int m_activeQueue;
	explicit EventManager() { m_activeQueue = 0; }
	~EventManager() { }
	bool VAddListener(const EventListenerDelegate& eventDelegate, const EventType& type);
	bool VRemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type);
	bool VTriggerEvent(const IEventDataPtr& pEvent) const;
	bool VQueueEvent(const IEventDataPtr& pEvent);
	bool VThreadSafeQueueEvent(const IEventDataPtr& pEvent);
	bool VAbortEvent(const EventType& type, bool allOfType = false);
	bool VUpdate(unsigned long maxMillis);
	static EventManager* Get();
};
void RegisterEngineScriptEvents();