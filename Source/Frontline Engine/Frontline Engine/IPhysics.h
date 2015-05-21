#pragma once
#include "FrontlineCommon.h"

struct PhysicsObject
{
	float m_mass;
	std::string m_material;
	StrongActorPtr m_gameActor;
	short m_collisionType;
	short m_collidesWith;
};

struct PhysicsMaterial
{
	float mf_restitution;
	float mf_friction;
};

class IPhysics
{
public:
	virtual bool VOnInit() = 0;
	virtual void VOnUpdate(float pf_deltaSeconds) = 0;

	virtual void VSyncVisibleScene() = 0;

	virtual void VAddRigidBody(PhysicsObject* po_physObject) = 0;
	virtual void VAddSphere(float pf_radius, WeakActorPtr pp_gameActor, const std::string& ps_density, const std::string& ps_material, short mi_collisionChannel = 1, short mi_collidesWith = 1) = 0;
	virtual void VAddCylinder(float pf_radius, float pf_height, WeakActorPtr pp_gameActor, const std::string& ps_density, const std::string& ps_material, short mi_collisionChannel = 1, short mi_collidesWith = 1) = 0;
	virtual void VAddBox(const glm::vec3& pf_dimensions, WeakActorPtr pp_gameActor, const std::string& ps_density, const std::string& ps_material, short mi_collisionChannel = 1, short mi_collidesWith =1) = 0;
	virtual void VRemoveActor(ActorID pi_id) = 0;

	virtual void VRenderDiagnostics() = 0;

	virtual void VCreateTrigger(PhysicsObject* po_physObject) = 0;
	virtual void VApplyForce(const glm::vec3& pv_dir, float pf_newtons, ActorID pi_id) = 0;
	virtual void VApplyTorque(const glm::vec3& pv_dir, float pf_newtons, ActorID pi_id) = 0;
	virtual bool VKinematicMove(const glm::mat4x4& pm_matrix, ActorID pi_id) = 0;

	virtual ~IPhysics() {};
};