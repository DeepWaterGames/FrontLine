#pragma once
#include "IResource.h"

typedef fastdelegate::FastDelegate1<unsigned int, char*> AllocationFunction;
class IResourceLoader
{
public:
	virtual std::string VGetPattern() = 0;
	virtual bool VUseRawFile() = 0;
	virtual unsigned int VGetLoadedResourceSize(
		char *rawBuffer, unsigned int rawSize) = 0;
	virtual bool VLoadResource(ResHandle& pp_retval, char *rawBuffer, const char* name, AllocationFunction alloc) = 0;
};