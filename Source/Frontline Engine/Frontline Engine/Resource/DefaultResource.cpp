#include "DefaultResource.h"
#include <new>

DefaultResource::DefaultResource(const std::string& ps_resname)
{
	ms_resourcename = ps_resname;
	std::transform(ms_resourcename.begin(), ms_resourcename.end(),
		ms_resourcename.begin(), (int(*)(int)) std::tolower);
}

std::string DefaultResource::getContents()
{
	return *contents;
}

void DefaultResource::setContents(std::string ps_contents)
{
	contents = FL_NEW std::string(ps_contents);
}

void DefaultResource::setContents(std::string* ps_contents)
{
	contents = ps_contents;
}

int DefaultResource::VGetSize()
{
	return contents->size();
}

int DefaultResource::VGetType()
{
	return RESOURCE_TEXT;
}

bool DefaultResourceLoader::VLoadResource(ResHandle& pp_retval, char *rawBuffer, const char* name, AllocationFunction alloc)
{
	void* resloc = alloc(sizeof(DefaultResource));
	DefaultResource* retval = FL_NEW(resloc) DefaultResource(name);

	void* strloc = alloc(sizeof(std::string));
	std::string* strval = FL_NEW(strloc) std::string(rawBuffer);

	retval->setContents(strval);

	pp_retval = ResHandle(retval);

	return true;
}