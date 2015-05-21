#pragma once
#include "..\FrontlineCommon.h"
#include <cctype>

class Resource;

typedef std::shared_ptr<Resource> ResHandle;

class Resource
{
public:
	std::string m_name;
	ResHandle GetHandle()
	{
		return ResHandle(this);
	}
	Resource(const std::string &name)
	{
		m_name = name;
		std::transform(m_name.begin(), m_name.end(),
			m_name.begin(), (int(*)(int)) std::tolower);
	}
};