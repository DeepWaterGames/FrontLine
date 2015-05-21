#pragma once
#include "..\IResourceFile.h"
#include "ZipFile.h"
class ZipResourceFile :
	public IResourceFile
{
protected:
	std::unique_ptr<ZipFile> mp_ZipFile;
public:
	virtual bool VOpen(const wchar_t* ps_filename) override;
	virtual int VGetResource(char** buffer, const char* ps_resname) override;
	virtual int VGetRawResourceSize(const char* ps_resname) override;
	virtual int VGetNumResources() const override;
	virtual bool VClose() override;
};