#pragma once
#include "..\FrontlineCommon.h"
#include "..\IGame.h"
#include "Actor.h"
#include "ActorComponent.h"

typedef ActorComponent *(*ActorComponentCreator)(void);
typedef std::map<std::string, ActorComponentCreator> ActorComponentCreatorMap;
// some actor typedefs to make our life easier
typedef unsigned long ActorId;
typedef std::shared_ptr<Actor> StrongActorPtr;
typedef std::shared_ptr<ActorComponent> StrongActorComponentPtr;
class ActorFactory
{
	ActorID m_lastActorID;
protected:
	GenericObjectFactory<ActorComponent, std::string> m_componentFactory;
	virtual ~ActorFactory() {};
public:
	ActorFactory(void);
	StrongActorPtr CreateActor(const char* p_actorResource);
	template<class t_ComponentClass>
	bool RegisterComponentType(std::string name)
	{
		m_componentFactory.Register<t_ComponentClass>(name);
		return true;
	}
	static ActorFactory* g_ActorFactory;
protected:
	virtual StrongActorComponentPtr CreateComponent(pugi::xml_node p_data);
private:
	ActorId GetNextActorId(void) { ++m_lastActorID; return m_lastActorID; }
};