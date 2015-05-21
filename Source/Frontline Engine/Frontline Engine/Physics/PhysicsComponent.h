#pragma once
#include "..\FrontlineCommon.h"
#include "..\IPhysics.h"
#include <PHYSICS\btBulletDynamicsCommon.h>
#include <PHYSICS\btBulletCollisionCommon.h>
#include "..\Actor\TransformComponent.h"
class PhysicsComponent :
	public ActorComponent
{
protected:
	std::weak_ptr<IPhysics> mp_gamePhysics;
	std::string ms_shape, ms_density, ms_material;
	glm::mat4 mm_transform;
	pugi::xml_node mn_shapeInfo;
	void BuildRigidBodyTransform(pugi::xml_node pn_transform);
	short m_collisionType, m_collidesWith;
public:
	const static std::string g_name;
	PhysicsComponent();
	virtual ~PhysicsComponent();
	virtual bool VInit(pugi::xml_node p_data) override;
	virtual void VPostInit() override;
	virtual void VUpdate(int p_deltaMs) {}

	virtual std::string VGetComponentName() override { return PhysicsComponent::g_name; }
	virtual ComponentID VGetComponentID() const override { return HashComponentName(g_name); };
	virtual pugi::xml_node VGenerateXML();
};