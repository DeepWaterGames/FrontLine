#pragma once
#include "..\FrontlineCommon.h"
#include "ResCache.h"
#include <XML/pugixml.hpp>
#include "Resource.h"

class XmlResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize) { return rawSize; }
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, ResHandle handle);
	virtual std::string VGetPattern() { return "*.xml"; }

	// convenience function
	static pugi::xml_node* LoadAndReturnRootXmlElement(const char* resourceString);
};

class XMLResource :
	public Resource
{
public:
	XMLResource(const std::string &name);
	~XMLResource();
};
