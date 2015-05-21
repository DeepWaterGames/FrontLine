#pragma once
#include "..\FrontlineCommon.h"
#include "ActorComponent.h"
#include <GLM/ext.hpp>
class TransformComponent :
	public ActorComponent
{
private:
	glm::mat4x4 mm_transform;
public:
	const static std::string g_name;
	TransformComponent();
	~TransformComponent();
	glm::vec3 FindPosition();
	glm::vec3 FindRotation();
	glm::vec3 FindScale();
	glm::mat4x4 GetTransform() { return mm_transform; }

	virtual bool VInit(pugi::xml_node p_data) override;
	virtual void VPostInit() override;
	virtual void VUpdate(int p_deltaMs) override {}
	virtual pugi::xml_node VGenerateXML() override { return pugi::xml_node(); }

	virtual std::string VGetComponentName() override { return TransformComponent::g_name; }
	virtual ComponentID VGetComponentID() const override { return HashComponentName(g_name); };
};