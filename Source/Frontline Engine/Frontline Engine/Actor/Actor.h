#pragma once
#include "../FrontlineCommon.h"
#include <map>
#include <memory>
#include <XML/pugixml.hpp>
#include <sstream>
#include "ActorComponent.h"

class Actor;

typedef unsigned long ActorID;
typedef std::shared_ptr<Actor> StrongActorPtr;
typedef std::weak_ptr<Actor> WeakActorPtr;

class Actor
{
	friend class ActorFactory;

	typedef std::map<ComponentID, StrongActorComponentPtr> ActorComponents;

	ActorID m_id;
	ActorComponents m_components;
public:
	explicit Actor(ActorID p_id);
	~Actor(void);

	bool Init(pugi::xml_node p_Data);
	void PostInit();
	void Destroy();

	void Update(int p_deltaMs);

	ActorID GetID() { return m_id; }

	std::string ToXML();

	template <class ComponentType>
	std::weak_ptr<ComponentType> GetComponent(ComponentID p_id)
	{
		ActorComponents::iterator l_findIt = m_components.find(p_id);
		if (l_findIt != m_components.end())
		{
			StrongActorComponentPtr lp_base(l_findIt->second);
			// cast to subclass version of the pointer
			std::shared_ptr<ComponentType> l_sub(std::static_pointer_cast<ComponentType>(lp_base));
			std::weak_ptr<ComponentType> l_weakSub(l_sub); // convert strong pointer to weak pointer
			return l_weakSub; // return the weak pointer
		}
		else
		{
			return std::weak_ptr<ComponentType>();
		}
	}

	template <class ComponentType>
	std::weak_ptr<ComponentType> GetComponent(std::string ps_name)
	{
		return GetComponent<ComponentType>(ActorComponent::HashComponentName(ps_name));
	}
private:
	void AddComponent(StrongActorComponentPtr p_component);
};