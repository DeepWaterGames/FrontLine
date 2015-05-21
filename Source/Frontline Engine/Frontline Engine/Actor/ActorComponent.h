#pragma once
#include "../FrontlineCommon.h"
#include "Actor.h"
#include <memory>
#include <XML/pugixml.hpp>

class ActorComponent;

typedef unsigned int ComponentID;
typedef std::shared_ptr<ActorComponent> StrongActorComponentPtr;

class ActorComponent
{
	friend class ActorFactory;
	friend class Actor;
protected:
	std::shared_ptr<Actor> m_owner;
public:
	virtual ~ActorComponent() {}

	virtual bool VInit(pugi::xml_node p_data) = 0;
	virtual void VPostInit() {}
	virtual void VUpdate(int p_deltaMs) {}

	virtual ComponentID VGetComponentID() const = 0;
	virtual std::string VGetComponentName() = 0;
	virtual pugi::xml_node VGenerateXML() = 0;
	static int HashComponentName(std::string ps_name)
	{
		int li_pos = ps_name.find("Component");
		ps_name.erase(li_pos, ps_name.length() - li_pos);
		int li_hash = 0;
		for (unsigned int i = 0; i < ps_name.length(); i++)
		{
			li_hash = li_hash * 31 + ps_name[i];
		}
		return li_hash;
	}
private:
	void SetOwner(std::shared_ptr<Actor> p_owner) { m_owner = p_owner; }
};