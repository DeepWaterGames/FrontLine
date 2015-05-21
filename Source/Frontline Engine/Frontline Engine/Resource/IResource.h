#pragma once
#include "..\FrontlineCommon.h"
#include <cctype>

class IResource;
typedef std::shared_ptr<IResource> ResHandle;

class IResource
{
protected:
	std::string ms_resourcename;
	int size;
public:
	IResource(const std::string &ps_resname = "")
	{
		ms_resourcename = ps_resname;
		std::transform(ms_resourcename.begin(), ms_resourcename.end(),
			ms_resourcename.begin(), (int(*)(int)) std::tolower);
	}
	~IResource();
	virtual int VGetType();
	virtual int VGetSize() = 0;
	ResHandle GetHandle() { return ResHandle(this); }
	std::string GetName() { return ms_resourcename; }
};