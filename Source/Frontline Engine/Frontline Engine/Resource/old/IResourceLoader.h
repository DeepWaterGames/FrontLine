#pragma once
#include "..\FrontlineCommon.h"
#include "Resource.h"

class IResourceLoader
{
public:
	virtual std::string VGetPattern() = 0;
	virtual bool VUseRawFile() = 0;
	virtual unsigned int VGetLoadedResourceSize(
		char *rawBuffer, unsigned int rawSize) = 0;
	virtual ResHandle VLoadResource(char *rawBuffer, char* name) = 0;
};