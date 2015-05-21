#pragma once
#include "Console.h"
#include <Windows.h>
class Win32Console :
	public Console
{
	HANDLE hConsole;
public:
	virtual void setColor(Color f) override;
	virtual void printf(const char* format, ...);
	Win32Console();
	~Win32Console();
};

