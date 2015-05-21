#include "ActorFactory.h"
#include <vector>

ActorFactory* ActorFactory::g_ActorFactory = FL_NEW ActorFactory();

ActorFactory::ActorFactory()
{
}

// --------------------------------------------------------------------------
// Function:      ActorFactory::CreateActor
// Purpose:       Create an actor from XML source
// Parameters:    The source XML
// --------------------------------------------------------------------------
StrongActorPtr ActorFactory::CreateActor(const char* actorResource)
{
	// Grab the root XML node
	pugi::xml_document l_xmlDoc;
	l_xmlDoc.load(actorResource);
	pugi::xml_node l_root = l_xmlDoc.root();
	if (!l_root)
	{
		ERRLOG("Actor", "Failed to create actor from resource : " + std::string(actorResource));
		return StrongActorPtr();
	}
	// create the actor instance
	StrongActorPtr l_actor(FL_NEW Actor(GetNextActorId()));
	if (!l_actor->Init(l_root))
	{
		ERRLOG("Actor", "Failed to initialize actor : " + std::string(actorResource));
		return StrongActorPtr();
	}
	// Loop through each child element and load the component
	for (pugi::xml_node l_node = l_root.first_child(); l_node; l_node = l_node.next_sibling())
	{
		StrongActorComponentPtr pComponent(CreateComponent(l_node));
		if (pComponent)
		{
			l_actor->AddComponent(pComponent);
			pComponent->SetOwner(l_actor);
		}
		else
		{
			return StrongActorPtr();
		}
	}
	// Now that the actor has been fully created, run the post init phase
	l_actor->PostInit();
	return l_actor;
}

// --------------------------------------------------------------------------
// Function:      ActorFactory::CreateComponent
// Purpose:       Create and initialize a component from XML.
// Parameters:    The XML node of the new component
// --------------------------------------------------------------------------
StrongActorComponentPtr ActorFactory::CreateComponent(pugi::xml_node l_data)
{
	const char* name = l_data.name();
	StrongActorComponentPtr pComponent(m_componentFactory.Create(name));

	// initialize the component if we found one
	if (pComponent)
	{
		if (!pComponent->VInit(l_data))
		{
			ERRLOG("Actor", "Component failed to initialize: " + std::string(name));
			return StrongActorComponentPtr();
		}
	}
	else
	{
		ERRLOG("Actor", "Couldn't find ActorComponent named " + std::string(name));
		return StrongActorComponentPtr();  // fail
	}

	// pComponent will be NULL if the component wasn't found.  This isn't necessarily an error since you might have a 
	// custom CreateComponent() function in a sub class.
	return pComponent;
}