#include "BulletPhysics.h"


BulletPhysics::BulletPhysics()
{

}


BulletPhysics::~BulletPhysics()
{
	for (int i = mp_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i)
	{
		btCollisionObject * const obj
			= mp_dynamicsWorld->getCollisionObjectArray()[i];
		RemoveCollisionObject(obj);
	}
	mmap_actorIDLookup.clear();
	mmap_rigidBodyLookup.clear();
	mset_lastTickCollisionPairs.clear();
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VOnInit
// Purpose:			Initialize Physics.
// Parameters:		None
// --------------------------------------------------------------------------
bool BulletPhysics::VOnInit()
{
	LoadXML();

	mp_collisionConfig = std::auto_ptr<btCollisionConfiguration>(FL_NEW btDefaultCollisionConfiguration());

	mp_collisionDispatcher = std::auto_ptr<btCollisionDispatcher>(FL_NEW btCollisionDispatcher(mp_collisionConfig.get()));

	mp_broadphase = std::auto_ptr<btBroadphaseInterface>(FL_NEW btDbvtBroadphase());

	mp_constraintSolver = std::auto_ptr<btConstraintSolver>(FL_NEW btSequentialImpulseConstraintSolver());

	mp_dynamicsWorld = std::auto_ptr<btDynamicsWorld>(FL_NEW btDiscreteDynamicsWorld(mp_collisionDispatcher.get(), mp_broadphase.get(), mp_constraintSolver.get(), mp_collisionConfig.get()));
	mp_dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

	mp_debugDrawer = std::auto_ptr<BulletDebugDrawer>(FL_NEW BulletDebugDrawer());

	if (!mp_collisionConfig.get())
	{
		ERRLOG("Physics", "BulletPhysics::VOnInit() failed: mp_collisionConfig not initialized.");
		return false;
	}
	if (!mp_collisionDispatcher.get())
	{
		ERRLOG("Physics", "BulletPhysics::VOnInit() failed: mp_collisionDispatcher not initialized.");
		return false;
	}
	if (!mp_broadphase.get())
	{
		ERRLOG("Physics", "BulletPhysics::VOnInit() failed: mp_broadphase not initialized.");
		return false;
	}
	if (!mp_constraintSolver.get())
	{
		ERRLOG("Physics", "BulletPhysics::VOnInit() failed: mp_constraintSolver not initialized.");
		return false;
	}
	if (!mp_dynamicsWorld.get())
	{
		ERRLOG("Physics", "BulletPhysics::VOnInit() failed: mp_dynamicsWorld not initialized.");
		return false;
	}
	if (!mp_debugDrawer.get())
	{
		ERRLOG("Physics", "BulletPhysics::VOnInit() failed: mp_debugDrawer not initialized.");
		return false;
	}

	mp_dynamicsWorld->setDebugDrawer(mp_debugDrawer.get());
	mp_dynamicsWorld->setInternalTickCallback(BulletInternalTickCallback);
	mp_dynamicsWorld->setWorldUserInfo(this);

	return true;
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VOnUpdate
// Purpose:			Tick the simulation.
// Parameters:		Elapsed time in seconds
// --------------------------------------------------------------------------
void BulletPhysics::VOnUpdate(float pf_deltaSeconds)
{
	mp_dynamicsWorld->stepSimulation(pf_deltaSeconds, 4); // Run the physics simulation
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VSyncVisibleScene
// Purpose:			Sync actor positions to physics locations.
// Parameters:		None
// --------------------------------------------------------------------------
void BulletPhysics::VSyncVisibleScene()
{
	for (auto lit_rigidBodyLookup = mmap_rigidBodyLookup.begin(); lit_rigidBodyLookup != mmap_rigidBodyLookup.end(); ++lit_rigidBodyLookup)
	{
		const ActorID li_id = lit_rigidBodyLookup->first;
		ActorMotionState const* lp_motionState = static_cast<ActorMotionState*>(lit_rigidBodyLookup->second->getMotionState());
	}
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VAddSphere
// Purpose:			Add a sphere to the simulation.
// Parameters:		Radius, weak pointer to actor, density lookup string, material lookup string, collision channel (1 bit selected), collision mask (binary OR of all collision channels)
// --------------------------------------------------------------------------
void BulletPhysics::VAddSphere(float const pf_radius, WeakActorPtr pp_gameActor, const std::string& ps_densityString, const std::string& ps_physicsMaterial, short mi_collisionChannel, short mi_collidesWith)
{
	StrongActorPtr lp_strongActor = MakeStrongPtr<Actor>(pp_gameActor);
	if (!lp_strongActor) return;

	btCollisionShape* lp_collisionShape = FL_NEW btSphereShape(pf_radius);
	float lf_density = findDensity(ps_densityString);
	const float lf_volume = (float) ((4.0F / 3.0F) * M_PI * pow(pf_radius, 3));
	const float lf_mass = lf_volume * lf_density;
	BulletPhysicsObject* lp_object = FL_NEW BulletPhysicsObject();
	lp_object->m_gameActor = lp_strongActor;
	lp_object->m_mass = lf_mass;
	lp_object->m_material = ps_physicsMaterial;
	lp_object->m_physShape = lp_collisionShape;
	lp_object->m_collisionType = mi_collisionChannel;
	lp_object->m_collidesWith = mi_collidesWith;
	VAddRigidBody(lp_object);
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VAddCylinder
// Purpose:			Add a cylinder to the simulation.
// Parameters:		Radius, height, weak pointer to actor, density lookup string, material lookup string, collision channel (1 bit selected), collision mask (binary OR of all collision channels)
// --------------------------------------------------------------------------
void BulletPhysics::VAddCylinder(float const pf_radius, float pf_height, WeakActorPtr pp_gameActor, const std::string& ps_densityString, const std::string& ps_physicsMaterial, short mi_collisionChannel, short mi_collidesWith)
{
	StrongActorPtr lp_strongActor = MakeStrongPtr<Actor>(pp_gameActor);
	if (!lp_strongActor) return;

	btCollisionShape* lp_collisionShape = FL_NEW btCylinderShape(btVector3(pf_radius, pf_height / 2, pf_radius));
	float lf_density = findDensity(ps_densityString);
	const float lf_volume = pf_radius * pf_radius * pf_height * M_PI;
	const float lf_mass = lf_volume * lf_density;
	BulletPhysicsObject* lp_object = FL_NEW BulletPhysicsObject();
	lp_object->m_gameActor = lp_strongActor;
	lp_object->m_mass = lf_mass;
	lp_object->m_material = ps_physicsMaterial;
	lp_object->m_physShape = lp_collisionShape;
	lp_object->m_collisionType = mi_collisionChannel;
	lp_object->m_collidesWith = mi_collidesWith;
	VAddRigidBody(lp_object);
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VAddBox
// Purpose:			Add a box to the simulation.
// Parameters:		Dimensions, weak pointer to actor, density lookup string, material lookup string, collision channel (1 bit selected), collision mask (binary OR of all collision channels)
// --------------------------------------------------------------------------
void BulletPhysics::VAddBox(const glm::vec3& pf_dimensions, WeakActorPtr pp_gameActor, const std::string& ps_density, const std::string& ps_physicsMaterial, short mi_collisionChannel, short mi_collidesWith)
{
	StrongActorPtr lp_strongActor = MakeStrongPtr<Actor>(pp_gameActor);
	if (!lp_strongActor) return;

	btCollisionShape* lp_collisionShape = FL_NEW btBoxShape(btVector3(pf_dimensions[0], pf_dimensions[1], pf_dimensions[2]));
	float lf_density = findDensity(ps_density);
	const float lf_volume = pf_dimensions[0] * pf_dimensions[1] * pf_dimensions[2];
	const float lf_mass = lf_volume * lf_density;
	BulletPhysicsObject* lp_object = FL_NEW BulletPhysicsObject();
	lp_object->m_gameActor = lp_strongActor;
	lp_object->m_mass = lf_mass;
	lp_object->m_material = ps_physicsMaterial;
	lp_object->m_physShape = lp_collisionShape;
	lp_object->m_collisionType = mi_collisionChannel;
	lp_object->m_collidesWith = mi_collidesWith;
	VAddRigidBody(lp_object);
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::SendNewCollisionPairEvent
// Purpose:			Helper function to notify the event system of a new collision pair.
// Parameters:		Manifold, rigid body 1, rigid body 2
// --------------------------------------------------------------------------
void BulletPhysics::SendNewCollisionPairEvent(btPersistentManifold* pp_manifold, btRigidBody* pp_body0, btRigidBody* pp_body1)
{
	std::shared_ptr<CollisionPairEventData> lp_eventdata;
	lp_eventdata = std::shared_ptr<CollisionPairEventData>(FL_NEW CollisionPairEventData());
	lp_eventdata->m_actor1 = mmap_actorIDLookup[pp_body0];
	lp_eventdata->m_actor2 = mmap_actorIDLookup[pp_body1];
	lp_eventdata->m_addOrRemove = true;
	IGame::gp_game->mp_eventManager->VQueueEvent(lp_eventdata);
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::SendRemoveCollisionPairEvent
// Purpose:			Helper function to notify the event system of a removed collision pair.
// Parameters:		Rigid body 1, rigid body 2
// --------------------------------------------------------------------------
void BulletPhysics::SendRemoveCollisionPairEvent(btRigidBody* pp_body0, btRigidBody* pp_body1)
{
	std::shared_ptr<CollisionPairEventData> lp_eventdata;
	lp_eventdata = std::shared_ptr<CollisionPairEventData>(FL_NEW CollisionPairEventData());
	lp_eventdata->m_actor1 = mmap_actorIDLookup[pp_body0];
	lp_eventdata->m_actor2 = mmap_actorIDLookup[pp_body1];
	lp_eventdata->m_addOrRemove = false;
	IGame::gp_game->mp_eventManager->VQueueEvent(lp_eventdata);
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VAddRigidBody
// Purpose:			Add a rigid body to the simulation.
// Parameters:		BulletPhysicsObject to be added
// --------------------------------------------------------------------------
void BulletPhysics::VAddRigidBody(PhysicsObject* po_physObject)
{
	BulletPhysicsObject* lo_objectBTCast = static_cast<BulletPhysicsObject*>(po_physObject);
	FL_ASSERT(lo_objectBTCast->m_gameActor);
	ActorID pid_actorID = lo_objectBTCast->m_gameActor->GetID();
	FL_ASSERT(mmap_rigidBodyLookup.find(pid_actorID) == mmap_rigidBodyLookup.end());

	PhysicsMaterial lmd_physMaterial(findMaterialData(lo_objectBTCast->m_material));

	btVector3 lv_localInertia;
	if (lo_objectBTCast->m_mass > 0.0F)
		lo_objectBTCast->m_physShape->calculateLocalInertia(lo_objectBTCast->m_mass, lv_localInertia);

	glm::mat4 lm_transform = glm::mat4(1);

	std::shared_ptr<TransformComponent> lp_transformComponent = MakeStrongPtr<TransformComponent>(lo_objectBTCast->m_gameActor->GetComponent<TransformComponent>(TransformComponent::g_name));
	FL_ASSERT(lp_transformComponent);
	if (!lp_transformComponent) return;

	lm_transform = lp_transformComponent->GetTransform();

	ActorMotionState* lp_motionState = FL_NEW ActorMotionState(lm_transform);

	btRigidBody::btRigidBodyConstructionInfo lc_constructInfo(lo_objectBTCast->m_mass, lp_motionState, lo_objectBTCast->m_physShape, lv_localInertia);

	lc_constructInfo.m_restitution = lmd_physMaterial.mf_restitution;
	lc_constructInfo.m_friction = lmd_physMaterial.mf_friction;

	btRigidBody* lp_newBody = FL_NEW btRigidBody(lc_constructInfo);

	mp_dynamicsWorld->addRigidBody(lp_newBody, po_physObject->m_collisionType, po_physObject->m_collidesWith);

	mmap_rigidBodyLookup[pid_actorID] = lp_newBody;
	mmap_actorIDLookup[lp_newBody] = pid_actorID;
	delete po_physObject;
}


// --------------------------------------------------------------------------
// Function:		BulletPhysics::VAddPointCloud
// Purpose:			Add a convex hull to the simulation.
// Parameters:		Array of vertices, number of vertices, density lookup string, material lookup string, collision channel (1 bit selected), collision mask (binary OR of all collision channels), mass
// --------------------------------------------------------------------------
void BulletPhysics::VAddPointCloud(glm::vec3* pp_verts, int pi_numPoints, WeakActorPtr pp_gameActor, const std::string ps_density, const std::string ps_physicsMaterial, float pi_mass)
{
	StrongActorPtr lp_strongActor = MakeStrongPtr<Actor>(pp_gameActor);
	if (!lp_strongActor) return;

	btConvexHullShape* const lp_collisionShape = FL_NEW btConvexHullShape();

	for (int i = 0; i < pi_numPoints; ++i)
	{
		lp_collisionShape->addPoint(btVector3(pp_verts[i][0], pp_verts[i][1], pp_verts[i][2]));
	}

	if (pi_mass == -1)
	{
		btVector3 lv_aabbMin, lv_aabbMax;

		lp_collisionShape->getAabb(btTransform::getIdentity(), lv_aabbMin, lv_aabbMax);

		btVector3 lv_aabbSize = lv_aabbMax - lv_aabbMin;
		float volume = lv_aabbSize.x() * lv_aabbSize.y() * lv_aabbSize.z();
		pi_mass = volume * findDensity(ps_density);
	}

	BulletPhysicsObject* lp_object = FL_NEW BulletPhysicsObject();
	lp_object->m_gameActor = lp_strongActor;
	lp_object->m_mass = pi_mass;
	lp_object->m_material = ps_physicsMaterial;
	lp_object->m_physShape = lp_collisionShape;
	VAddRigidBody(lp_object);
}

/* TODO: Redo VAdd- functions to return PhysicsObject* */
// --------------------------------------------------------------------------
// Function:		BulletPhysics::VCreateTrigger
// Purpose:			Add a trigger to the simulation.
// Parameters:		BulletPhysicsObject to be added as trigger
// --------------------------------------------------------------------------
void BulletPhysics::VCreateTrigger(PhysicsObject* po_physObject)
{
	po_physObject->m_mass = 0;
	BulletPhysicsObject* lo_objectBTCast = static_cast<BulletPhysicsObject*>(po_physObject);
	FL_ASSERT(lo_objectBTCast->m_gameActor);
	ActorID pid_actorID = lo_objectBTCast->m_gameActor->GetID();
	FL_ASSERT(mmap_rigidBodyLookup.find(pid_actorID) == mmap_rigidBodyLookup.end());

	btVector3 lv_localInertia;
	if (lo_objectBTCast->m_mass > 0.0F)
		lo_objectBTCast->m_physShape->calculateLocalInertia(lo_objectBTCast->m_mass, lv_localInertia);

	glm::mat4 lm_transform = glm::mat4(1);

	std::shared_ptr<TransformComponent> lp_transformComponent = MakeStrongPtr<TransformComponent>(lo_objectBTCast->m_gameActor->GetComponent<TransformComponent>(TransformComponent::g_name));
	FL_ASSERT(lp_transformComponent);
	if (!lp_transformComponent) return;

	lm_transform = lp_transformComponent->GetTransform();

	ActorMotionState* lp_motionState = FL_NEW ActorMotionState(lm_transform);

	btRigidBody::btRigidBodyConstructionInfo lc_constructInfo(lo_objectBTCast->m_mass, lp_motionState, lo_objectBTCast->m_physShape, lv_localInertia);

	btRigidBody* lp_newBody = new btRigidBody(lc_constructInfo);

	mp_dynamicsWorld->addRigidBody(lp_newBody);

	lp_newBody->setCollisionFlags(lp_newBody->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);

	mmap_rigidBodyLookup[pid_actorID] = lp_newBody;
	mmap_actorIDLookup[lp_newBody] = pid_actorID;
	delete po_physObject;
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VApplyForce
// Purpose:			Apply force to an actor.
// Parameters:		Direction, power in newtons, actor id
// --------------------------------------------------------------------------
void BulletPhysics::VApplyForce(const glm::vec3& pv_dir, float pf_newtons, ActorID pi_id)
{
	btRigidBody* lp_body = mmap_rigidBodyLookup[pi_id];
	if (!lp_body) return;
	FL_ASSERT(lp_body);
	lp_body->applyCentralImpulse(btVector3(pv_dir.x * pf_newtons, pv_dir.y * pf_newtons, pv_dir.z * pf_newtons));
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VApplyTorque
// Purpose:			Apply torque to a specified actor.
// Parameters:		Direction, power in newtons, actor id
// --------------------------------------------------------------------------
void BulletPhysics::VApplyTorque(const glm::vec3& pv_dir, float pf_newtons, ActorID pi_id)
{
	btRigidBody* lp_body = mmap_rigidBodyLookup[pi_id];
	if (!lp_body) return;
	FL_ASSERT(lp_body);
	lp_body->applyTorqueImpulse(btVector3(pv_dir.x * pf_newtons, pv_dir.y * pf_newtons, pv_dir.z * pf_newtons));
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VStopActor
// Purpose:			Halt an actor's motion.
// Parameters:		Actor id to stop
// --------------------------------------------------------------------------
void BulletPhysics::VStopActor(ActorID pi_id)
{
	VSetVelocity(pi_id, glm::vec3(0, 0, 0));
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VSetVelocity
// Purpose:			Set an actor's velocity
// Parameters:		Actor id, velocity
// --------------------------------------------------------------------------
void BulletPhysics::VSetVelocity(ActorID pi_id, glm::vec3& pv_velocity)
{
	btRigidBody* lp_body = mmap_rigidBodyLookup[pi_id];
	if (!lp_body) return;
	lp_body->setLinearVelocity(btVector3(pv_velocity[0], pv_velocity[1], pv_velocity[2]));
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::findDensity
// Purpose:			Find the value of a density string
// Parameters:		Name of the density material
// --------------------------------------------------------------------------
float BulletPhysics::findDensity(const std::string& ps_name)
{
	return mmap_densityTable[ps_name];
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::findMaterialData
// Purpose:			Find the value of a material string
// Parameters:		Name of the material
// --------------------------------------------------------------------------
PhysicsMaterial BulletPhysics::findMaterialData(const std::string& ps_name)
{
	return mmap_materialTable[ps_name];
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VRemoveActor
// Purpose:			Remove an actor from the simulation
// Parameters:		Actor id to be removed
// --------------------------------------------------------------------------
void BulletPhysics::VRemoveActor(ActorID pi_id)
{
	mmap_actorIDLookup.erase(mmap_rigidBodyLookup[pi_id]);
	delete mmap_rigidBodyLookup[pi_id];
	mmap_rigidBodyLookup.erase(pi_id);
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VRenderDiagnostics
// Purpose:			Render debug drawing
// Parameters:		None
// --------------------------------------------------------------------------
void BulletPhysics::VRenderDiagnostics()
{
	mp_dynamicsWorld->debugDrawWorld();
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::RemoveCollisionObject
// Purpose:			Helper function to remove a specific object from the simulation
// Parameters:		Collision object to be removed
// --------------------------------------------------------------------------
void BulletPhysics::RemoveCollisionObject(btCollisionObject* pp_toRemove)
{
	mp_dynamicsWorld->removeCollisionObject(pp_toRemove);
	delete pp_toRemove;
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::LoadXML
// Purpose:			Load physics settings
// Parameters:		None
// --------------------------------------------------------------------------
void BulletPhysics::LoadXML()
{
	PhysicsMaterial lm_defaultMaterial;
	lm_defaultMaterial.mf_friction = 0.5;
	lm_defaultMaterial.mf_restitution = 0.2;
	mmap_densityTable["Default"] = 1.0F;
	mmap_materialTable["Default"] = lm_defaultMaterial;

	mmap_densityTable["Static"] = 0.0F;

	PhysicsMaterial lm_bouncyBall;
	lm_bouncyBall.mf_friction = 0.5;
	lm_bouncyBall.mf_restitution = 1;
	mmap_materialTable["Bouncy"] = lm_bouncyBall;
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::BulletInternalTickCallback
// Purpose:			Static tick callback - called every step by bullet
// Parameters:		Physics world, elapsed time in seconds
// --------------------------------------------------------------------------
void BulletPhysics::BulletInternalTickCallback(btDynamicsWorld* pp_dynamicsWorld, btScalar pf_timeStep)
{
	FL_ASSERT(pp_dynamicsWorld);
	FL_ASSERT(pp_dynamicsWorld->getWorldUserInfo());
	BulletPhysics* lp_physics = static_cast<BulletPhysics*>(pp_dynamicsWorld->getWorldUserInfo());
	int numManifolds = pp_dynamicsWorld->getDispatcher()->getNumManifolds();

	std::set<CollisionPair> lset_currentTickCollisionPairs;

	for (int i = 0; i<numManifolds; i++)
	{
		btPersistentManifold* contactManifold = pp_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btRigidBody* obA = (btRigidBody*) (contactManifold->getBody0());
		btRigidBody* obB = (btRigidBody*) (contactManifold->getBody1());

		btRigidBody* obASort = (obA > obB) ? obA : obB;
		btRigidBody* obBSort = (obA > obB) ? obB : obA;

		CollisionPair lpair_collisionPair = std::make_pair(obASort, obBSort);


		if (lp_physics->mset_lastTickCollisionPairs.find(lpair_collisionPair) == lp_physics->mset_lastTickCollisionPairs.end() && lset_currentTickCollisionPairs.find(lpair_collisionPair) == lset_currentTickCollisionPairs.end())
		{
			lp_physics->SendNewCollisionPairEvent(contactManifold, obASort, obBSort);
		}
		lset_currentTickCollisionPairs.insert(lpair_collisionPair);
	}

	std::set<CollisionPair> lset_removedPairs;
	std::set_difference(lp_physics->mset_lastTickCollisionPairs.begin(), lp_physics->mset_lastTickCollisionPairs.end(), lset_currentTickCollisionPairs.begin(), lset_currentTickCollisionPairs.end(), std::inserter(lset_removedPairs, lset_removedPairs.begin()));
	for (std::set<CollisionPair>::iterator i = lset_removedPairs.begin(); i != lset_removedPairs.end(); i++)
	{
		lp_physics->SendRemoveCollisionPairEvent(i->first, i->second);
	}
	lp_physics->mset_lastTickCollisionPairs = lset_currentTickCollisionPairs;
}

// --------------------------------------------------------------------------
// Function:		BulletPhysics::VKinematicMove
// Purpose:			Set a new transformation for an actor
// Parameters:		New transform, actor id
// --------------------------------------------------------------------------
bool BulletPhysics::VKinematicMove(const glm::mat4x4& pm_matrix, ActorID pi_id)
{
	return true;
}