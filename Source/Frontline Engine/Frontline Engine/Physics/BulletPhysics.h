#pragma once
#include "..\FrontlineCommon.h"
#include "..\IPhysics.h"
#include "..\Actor\TransformComponent.h"
#include "ActorMotionState.h"
#include "BulletDebugDrawer.h"
#include "..\Event\AddCollisionPairEventData.h"
#include <set>
#include <cmath>
#include <algorithm>

struct BulletPhysicsObject :
	public PhysicsObject
{
	btCollisionShape* m_physShape;
};

enum collisiontypes
{
	COL_NOTHING = 0x00,
	COL_ENVIRONMENT = 0x01,
	COL_PLAYER = 0x02,
	COL_VEHICLE = 0x04
};

class BulletPhysics :
	public IPhysics
{
	std::auto_ptr<btDynamicsWorld>						mp_dynamicsWorld;
	std::auto_ptr<btBroadphaseInterface>				mp_broadphase;
	std::auto_ptr<btCollisionDispatcher>				mp_collisionDispatcher;
	std::auto_ptr<btConstraintSolver>					mp_constraintSolver;
	std::auto_ptr<btCollisionConfiguration>				mp_collisionConfig;
	std::auto_ptr<BulletDebugDrawer>					mp_debugDrawer;

	typedef std::map<std::string, float> DensityTable;
	typedef std::map<std::string, PhysicsMaterial> MaterialTable;

	DensityTable mmap_densityTable;
	MaterialTable mmap_materialTable;

	void LoadXML();

	std::map<ActorID, btRigidBody*> mmap_rigidBodyLookup;
	std::map<btRigidBody*, ActorID> mmap_actorIDLookup;

	typedef std::pair<btRigidBody*, btRigidBody*> CollisionPair;
	std::set<CollisionPair> mset_lastTickCollisionPairs;

	void SendNewCollisionPairEvent(btPersistentManifold* pp_manifold, btRigidBody* pp_body0, btRigidBody* pp_body1);
	void SendRemoveCollisionPairEvent(btRigidBody* pp_body0, btRigidBody* pp_body1);
	void RemoveCollisionObject(btCollisionObject* pp_toRemove);

	static void BulletInternalTickCallback(btDynamicsWorld* pp_world, btScalar pf_timeStep);

public:
	float findDensity(const std::string& ps_name);
	PhysicsMaterial findMaterialData(const std::string& ps_name);

	virtual bool VOnInit() override;
	virtual void VSyncVisibleScene() override;
	virtual void VOnUpdate(float pf_deltaSeconds);

	virtual void VAddRigidBody(PhysicsObject* po_physObject) override;
	virtual void VAddCylinder(float pf_radius, float pf_height, WeakActorPtr pp_gameActor, const std::string& ps_density, const std::string& ps_material, short mi_collisionChannel, short mi_collidesWith);
	virtual void VAddSphere(float pf_radius, WeakActorPtr pp_gameActor, const std::string& ps_density, const std::string& ps_material, short mi_collisionChannel, short mi_collidesWith) override;
	virtual void VAddBox(const glm::vec3& pf_dimensions, WeakActorPtr pp_gameActor, const std::string& ps_density, const std::string& ps_material, short mi_collisionChannel, short mi_collidesWith);
	void VAddPointCloud(glm::vec3* pp_verts, int pi_numPoints, WeakActorPtr pp_gameActor, const std::string ps_density, const std::string ps_material, float mass = -1);

	virtual void VRemoveActor(ActorID pi_id);

	virtual void VRenderDiagnostics() override;

	virtual void VCreateTrigger(PhysicsObject* po_physObject);
	virtual void VApplyForce(const glm::vec3& pv_dir, float pf_newtons, ActorID pi_id);
	virtual void VApplyTorque(const glm::vec3& pv_dir, float pf_newtons, ActorID pi_id);
	virtual bool VKinematicMove(const glm::mat4x4& pm_matrix, ActorID pi_id);

	void VStopActor(ActorID pi_id);
	void VSetVelocity(ActorID pi_id, glm::vec3& pv_velocity);

	BulletPhysics();
	~BulletPhysics();
};