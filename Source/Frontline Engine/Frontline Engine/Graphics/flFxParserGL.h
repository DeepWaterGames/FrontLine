#pragma once
#include "flFxParser.h"

class flFxParserGL : public flFxParser
{
public:
	flFxParserGL();
	~flFxParserGL();

	bool HandleEffectNode(pugi::xml_node node); //no references
	bool HandleTechniqueNode(pugi::xml_node node);
	bool HandlePassNode(pugi::xml_node node);
	bool HandleShaderBodyNode(pugi::xml_node node);
	bool HandleResourceBodyNode(pugi::xml_node node);
	bool HandleFrameBufferObjectBodyNode(pugi::xml_node node);
	bool HandleStateNode(pugi::xml_node node);

	static flFxParserGL* Get();
};

