#include "GLenum.h"

static GLenums enums;
GLenums::GLenums() {
	REGISTER_AS_STRING(GL_TRUE, "GL_TRUE");
	REGISTER_AS_STRING(GL_FALSE, "GL_FALSE");
	REGISTER_AS_STRING(GL_ONE, "GL_ONE");
	REGISTER_AS_STRING(GL_ONE_MINUS_DST_ALPHA, "GL_ONE_MINUS_DST_ALPHA");
	//texture states
	REGISTER_AS_STRING(GL_LINEAR, "GL_LINEAR");
	REGISTER_AS_STRING(GL_REPEAT, "GL_REPEAT");
}
GLenums::~GLenums() {

}
const char* GLenums::ToString(GLenum state) {
	for (auto iter = m_Enums.begin(); iter != m_Enums.end(); iter++) {
		if (iter->first == state) {
			return iter->second;
		}
	}
	return "GL_INVALID_ENUM";
}
GLenum GLenums::ToGLenum(const char* state) {
	for (auto iter = m_Enums.begin(); iter != m_Enums.end(); iter++) {
		if (strcmp(iter->second, state) == 0) {
			return iter->first;
		}
	}
	return GL_INVALID_ENUM;
}
GLenums GLenums::Get() {
	return enums;
}