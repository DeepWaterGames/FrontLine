#include "Math.h"

// --------------------------------------------------------------------------
// Function:		btTransformToGLM
// Purpose:			Convert a bullet transform into a glm matrix
// Parameters:		A const reference to the bullet transform
// --------------------------------------------------------------------------
const glm::mat4x4 btTransformToGLM(const btTransform& pt_transform)
{
	glm::mat4x4 lm_retVal;
	pt_transform.getOpenGLMatrix(glm::value_ptr(lm_retVal));
	return lm_retVal;
}

// --------------------------------------------------------------------------
// Function:		glmMatToBT
// Purpose:			Convert a glm matrix into a bullet transform
// Parameters:		A const reference to the glm matrix
// --------------------------------------------------------------------------
const btTransform glmMatToBT(const glm::mat4x4& pm_matrix)
{
	btTransform lt_retval;
	lt_retval.setFromOpenGLMatrix(glm::value_ptr(pm_matrix));
	return lt_retval;
}

// --------------------------------------------------------------------------
// Function:		getTranslationFromMat
// Purpose:			Find the translation component of a transformation matrix
// Parameters:		The glm matrix
// --------------------------------------------------------------------------
const glm::vec3 getTranslationFromMat(const glm::mat4x4 pm_matrix)
{
	return glm::vec3(pm_matrix[0][0], pm_matrix[1][1], pm_matrix[2][2]);
}

// --------------------------------------------------------------------------
// Function:		getScaleFromMat
// Purpose:			Find the scale component of a transformation matrix
// Parameters:		The glm matrix
// --------------------------------------------------------------------------
const glm::vec3 getScaleFromMat(const glm::mat4x4 pm_matrix)
{
	return glm::vec3(pm_matrix[3][0], pm_matrix[3][1], pm_matrix[3][2]);
}

// --------------------------------------------------------------------------
// Function:		fastInverseSqrRoot
// Purpose:			Find 1/sqr(x) quickly - used for normalizing vectors
// Parameters:		X
// --------------------------------------------------------------------------
float fastInverseSqrRoot(float x)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = x * 0.5F;
	y = x;
	i = *(long *) &y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *) &i;
	y = y * (threehalfs - (x2 * y * y));
	return y;
}

// --------------------------------------------------------------------------
// Function:		normalize
// Purpose:			Normalize a vector - make its length 1
// Parameters:		A reference to the vector
// --------------------------------------------------------------------------
inline void normalize(glm::vec3& vec)
{
	glm::inversesqrt(10);
	vec *= fastInverseSqrRoot(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}