#pragma once
#include "Resource.h"
#include "IResourceLoader.h"

class TextResource :
	public Resource
{
	std::string ms_contents;
public:
	~TextResource();
	std::string GetContents();
	void SetContents(std::string ps_contents);
};

class TextResourceLoader :
	public IResourceLoader
{

};