#pragma once
#include "..\FrontlineCommon.h"
#include <PHYSICS\btBulletDynamicsCommon.h>
#include <GLM\ext.hpp>
class ActorMotionState :
	public btMotionState
{
public:
	glm::mat4x4 mm_worldToPositionTransform;

	ActorMotionState(const glm::mat4x4& pm_initialTransform) : mm_worldToPositionTransform(pm_initialTransform) { };

	virtual void getWorldTransform(btTransform& pt_worldTrans) const override;

	virtual void setWorldTransform(const btTransform& pt_worldTrans) override;

	ActorMotionState();
	~ActorMotionState();
};