#include "../FrontlineCommon.h"
#include <sstream>
#include <iostream>

Actor::Actor(ActorID p_id)
{
	m_id = p_id;
}


Actor::~Actor()
{
}

// --------------------------------------------------------------------------
// Function:      Actor::Init
// Purpose:       Initialize an actor.
// Parameters:    The actor xml node
// --------------------------------------------------------------------------
bool Actor::Init(pugi::xml_node pData)
{
	std::stringstream l_msg;
	l_msg << "Initializing Actor " << m_id;
	MSGLOG("ACTOR", l_msg.str());
	return true;
	m_components;
}

// --------------------------------------------------------------------------
// Function:      Actor::PostInit
// Purpose:       Finish component initialization.
// Parameters:    None
// --------------------------------------------------------------------------
void Actor::PostInit(void)
{
	for (ActorComponents::iterator it = m_components.begin(); it != m_components.end(); ++it)
	{
		it->second->VPostInit();
	}
}

// --------------------------------------------------------------------------
// Function:      Actor::Destroy
// Purpose:       Destroy an actor and its components.
// Parameters:    None
// --------------------------------------------------------------------------
void Actor::Destroy(void)
{
	m_components.clear();
}

// --------------------------------------------------------------------------
// Function:      Actor::ToXML
// Purpose:       Generate the actor XML.
// Parameters:    None
// --------------------------------------------------------------------------
std::string Actor::ToXML()
{
	pugi::xml_document l_outDoc;

	// Actor element
	pugi::xml_node l_actorElement = pugi::xml_node();

	// components
	for (auto it = m_components.begin(); it != m_components.end(); ++it)
	{
		StrongActorComponentPtr l_component = it->second;
		pugi::xml_node l_componentElement = l_component->VGenerateXML();
		l_actorElement.append_child().set_name(l_componentElement.name());
		l_actorElement.child(l_componentElement.name()) = l_componentElement;
	}

	std::stringbuf l_retValBuffer;
	std::ostream l_xmlStream(&l_retValBuffer);

	l_outDoc.save(l_xmlStream);

	return l_retValBuffer.str();
}

// --------------------------------------------------------------------------
// Function:      Actor::AddComponent
// Purpose:       Attach a component to the actor.
// Parameters:    A shared pointer to the component
// --------------------------------------------------------------------------
void Actor::AddComponent(StrongActorComponentPtr pComponent)
{
	std::pair<ActorComponents::iterator, bool> success = m_components.insert(std::make_pair(pComponent->VGetComponentID(), pComponent));
	FL_ASSERT(success.second);
}

void Actor::Update(int p_deltaMs)
{
	for (ActorComponents::iterator it = m_components.begin(); it != m_components.end(); ++it)
	{
		it->second->VUpdate(p_deltaMs);
	}
}