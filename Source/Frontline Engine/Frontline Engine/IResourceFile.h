#pragma once
#include "Resource\IResource.h"
class IResourceFile
{
protected:
	std::wstring ms_filename;
public:
	virtual bool VOpen(const wchar_t* ps_filename) = 0;
	virtual int VGetResource(char** buffer, const char* ps_resname) = 0;
	virtual int VGetRawResourceSize(const char* ps_resname) = 0;
	virtual int VGetNumResources() const = 0;
	virtual bool VClose() = 0;
	virtual std::wstring GetFilename() { return ms_filename; }
	virtual ~IResourceFile() {}
};