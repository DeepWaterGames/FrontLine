#pragma once
#include "..\Graphics\Color.h"
class Console
{
public:
	virtual void setColor(Color f) = 0;
	virtual void printf(const char* format, ...) = 0;
	virtual ~Console() {};
};