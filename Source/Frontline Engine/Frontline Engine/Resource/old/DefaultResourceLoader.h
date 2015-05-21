#pragma once
#include "..\FrontlineCommon.h"
#include "IResourceLoader.h"

class DefaultResource :
	public Resource
{
	std::string contents;
public:
	DefaultResource(std::string ps_name);
	std::string getContents();
	void setContents(std::string ps_contents);
};

class DefaultResourceLoader :
	public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize) {
		return rawSize;
	}
	virtual ResHandle VLoadResource(char *rawBuffer, char* name) {
		DefaultResource* lp_res = FL_NEW DefaultResource(std::string(name));
		lp_res->setContents(std::string(rawBuffer));
		return ResHandle(lp_res);
	}
	virtual std::string VGetPattern() { return "*"; }
};