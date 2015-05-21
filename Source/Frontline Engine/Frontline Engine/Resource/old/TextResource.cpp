#include "TextResource.h"

TextResource::~TextResource()
{
}

std::string TextResource::GetContents()
{
	return ms_contents;
}

void TextResource::SetContents(std::string ps_contents)
{
	ms_contents = ps_contents;
}