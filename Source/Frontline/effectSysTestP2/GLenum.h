#pragma once
#include <map>
#include <utility>
#include <vector>
#include "flFxConfig.h"
#include "GL.h"

#define REGISTER_AS_STRING(x,y) m_Enums.push_back(std::pair<GLenum, const char*>(x,y))

class GLenums {
private:
	std::vector<std::pair<GLenum, const char*>> m_Enums;
public:
	GLenums();
	~GLenums();
	const char* ToString(GLenum state);
	GLenum ToGLenum(const char* state);
	static GLenums Get();
};