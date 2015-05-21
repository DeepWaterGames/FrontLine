#pragma once
#include "IResourceLoader.h"
#include "ResCache.h"

#define RESOURCE_TEXT 0x00000001

class DefaultResource :
	public IResource
{
	std::string* contents;
public:
	DefaultResource(const std::string &ps_resname = "");
	std::string getContents();
	void setContents(std::string ps_contents);
	void setContents(std::string* ps_contents);
	virtual int VGetSize() override;
	virtual int VGetType() override;
};

class DefaultResourceLoader :
	public IResourceLoader
{
public:
	virtual std::string VGetPattern() { return "*"; }
	virtual bool VUseRawFile();
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(ResHandle& pp_retval, char *rawBuffer, const char* name, AllocationFunction alloc);
};