#pragma once
#include "..\FrontlineCommon.h"
#include <PHYSICS/btBulletDynamicsCommon.h>
#include <GLM/ext.hpp>
#define max(a, b) a > b ? a : b

const glm::mat4x4 btTransformToGLM(const btTransform& pt_transform);

const btTransform glmMatToBT(const glm::mat4x4& pm_matrix);

const glm::vec3 getTranslationFromMat(const glm::mat4x4 pm_matrix);
const glm::vec3 getScaleFromMat(const glm::mat4x4 pm_matrix);

inline float fastInverseSqrRoot(float x);
inline void normalize(glm::vec3& vec);