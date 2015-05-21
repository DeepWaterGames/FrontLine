#pragma once
#include "..\FrontlineCommon.h"
#include <PHYSICS\btBulletDynamicsCommon.h>


class CollisionPairEventData :
	public BaseEventData
{
public:
	static EventType sk_eventType;
	ActorID m_actor1, m_actor2;
	bool m_addOrRemove;
	CollisionPairEventData();
	~CollisionPairEventData();
	virtual const EventType& VGetEventType(void) const	override { return sk_eventType; };
	virtual void VSerialize(std::ostrstream &out) const	override { }
	virtual void VDeserialize(std::istrstream& in) override { }
	virtual const char* GetName(void) const { return "EvtData_AddCollisionPair"; };
};