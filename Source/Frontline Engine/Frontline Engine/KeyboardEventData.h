#pragma once
#include "FrontlineCommon.h"
#include "Event\EventManager.h"
class KeyboardEventData :
	public BaseEventData
{
public:
	static EventType sk_eventType;
	bool m_down;
	int code;
	KeyboardEventData();
	~KeyboardEventData();
	virtual const EventType& VGetEventType(void) const	override { return sk_eventType; };
	virtual void VSerialize(std::ostrstream &out) const	override { }
	virtual void VDeserialize(std::istrstream& in) override { }
	virtual const char* GetName(void) const { return "EvtData_Keyboard"; };
};