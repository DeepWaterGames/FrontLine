#include "XMLResource.h"


XMLResource::~XMLResource()
{
}

bool XmlResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, ResHandle handle)
{
	pugi::xml_document* lp_doc = FL_NEW pugi::xml_document();
	pugi::xml_parse_result lr_result = lp_doc->load(rawBuffer);
	if (lr_result.status == pugi::xml_parse_status::status_ok)
	{

	}
	else return false;
}
	static pugi::xml_node** LoadAndReturnRootXmlElement(const char* resourceString);