#pragma once
#include "..\FrontlineCommon.h"
#include "../IGame.h"
#include "Console.h"
#include <vector>

class Log
{
	std::vector<std::pair<int, std::string>> m_log;
	Console* c;
public:
	Log();
	~Log();
	void logError(std::string p_cat, std::string p_error);
	void logWarning(std::string p_cat, std::string p_warning);
	void logOther(std::string p_cat, std::string p_message);
	std::vector<std::pair<int, std::string>> dumpLog();
	bool dumpToFile(std::string p_fileName);
	bool dumpToFile();
};