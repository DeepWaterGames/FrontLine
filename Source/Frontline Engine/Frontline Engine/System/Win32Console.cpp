#include "Win32Console.h"
#include <stdarg.h>
#include <cstdio>
#include <string>

Win32Console::Win32Console()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}


Win32Console::~Win32Console()
{
}

void Win32Console::printf(const char* format, ...)
{
	va_list arg;
	// TODO Color handling
	va_start(arg, format);
	vfprintf(stdout, format, arg);
	va_end(arg);
}

void Win32Console::setColor(Color f)
{
	SetConsoleTextAttribute(hConsole, f.to8Bit());
}