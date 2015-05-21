#include "ActorMotionState.h"

ActorMotionState::ActorMotionState()
{
}


ActorMotionState::~ActorMotionState()
{
}

// --------------------------------------------------------------------------
// Function:      ActorMotionState::setWorldTransform
// Purpose:       Set the bullet transform of the motion state.
// Parameters:    The new bullet motion state
// --------------------------------------------------------------------------
void ActorMotionState::setWorldTransform(const btTransform& pt_worldTrans)
{
	mm_worldToPositionTransform = btTransformToGLM(pt_worldTrans);
}

// --------------------------------------------------------------------------
// Function:      ActorMotionState::getWorldTransform
// Purpose:       Get the bullet transform of the motion state.
// Parameters:    A reference to the bullet transform
// --------------------------------------------------------------------------
void ActorMotionState::getWorldTransform(btTransform& pt_worldTrans) const
{
	pt_worldTrans = glmMatToBT(mm_worldToPositionTransform);
}