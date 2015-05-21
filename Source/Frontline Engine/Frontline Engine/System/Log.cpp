#include "Log.h"
#include <fstream>
#include <cctype>
#include "Win32Console.h"

Log::Log()
{
	c = new Win32Console();
}


Log::~Log()
{
}

// --------------------------------------------------------------------------
// Function:		Log::LogError
// Purpose:			Log an error message
// Parameters:		The message to be logged
// --------------------------------------------------------------------------
void Log::logError(std::string p_cat, std::string p_error)
{
	std::transform(p_cat.begin(), p_cat.end(), p_cat.begin(), (int(*)(int)) std::toupper);
	m_log.push_back(std::pair<int, std::string>(0xFFFFFFFF, "[" + Time::g_Time->getFormattedDateTime() + "] [ERROR] [" + p_cat + "] " + p_error));
}

// --------------------------------------------------------------------------
// Function:		Log::LogWarning
// Purpose:			Log a warning message
// Parameters:		The message to be logged
// --------------------------------------------------------------------------
void Log::logWarning(std::string p_cat, std::string p_warning)
{
	std::transform(p_cat.begin(), p_cat.end(), p_cat.begin(), (int(*)(int)) std::toupper);
	m_log.push_back(std::pair<int, std::string>(1, "{color:1,1,0}[" + Time::g_Time->getFormattedDateTime() + "] [WARNING] [" + p_cat + "] " + p_warning));
}

// --------------------------------------------------------------------------
// Function:		Log::LogOther
// Purpose:			Log a debug message
// Parameters:		The message category, the message to be logged
// --------------------------------------------------------------------------
void Log::logOther(std::string p_cat, std::string p_message)
{
	std::transform(p_cat.begin(), p_cat.end(), p_cat.begin(), (int(*)(int)) std::toupper);
	m_log.push_back(std::pair<int, std::string>(0, "{color:1,1,0}[" + Time::g_Time->getFormattedDateTime() + "] [" + p_cat + "] " + p_message));
}

// --------------------------------------------------------------------------
// Function:		Log::dumpLog
// Purpose:			Dump the contents of the log to a string and return them
// Parameters:		None
// --------------------------------------------------------------------------
std::vector<std::pair<int, std::string>> Log::dumpLog()
{
	auto l_retval = m_log;
	m_log.clear();
	for (int i = 0; i < l_retval.size(); i++) { // TODO Get rid of this, it is for testing the console
		c->printf(l_retval[i].second.append("\n").c_str());
	}
	return l_retval;
}

// --------------------------------------------------------------------------
// Function:		Log::dumpToFile
// Purpose:			Dump the contents of the log to a specified file
// Parameters:		The name of the file
// --------------------------------------------------------------------------
bool Log::dumpToFile(std::string p_fileName)
{
	std::ofstream l_logFile;
	l_logFile.open(p_fileName);

	std::string ls_text;
	for (int i = 0; i < m_log.size(); i++)
	{
		ls_text.append(m_log[i].second);
		ls_text.append("\n");
	}

	if (l_logFile.is_open())
	{
		l_logFile << ls_text;
		l_logFile.close();
		m_log.clear();
		return true;
	}
	logError("Log", "Log dump failed: cannot open file " + p_fileName + ". Dumping instead to default file (logs\\M-D-Y H.M.S");
	return false;
}

// --------------------------------------------------------------------------
// Function:		Log::dumpToFile
// Purpose:			Dump the contents of the log to the default file
// Parameters:		None
// --------------------------------------------------------------------------
bool Log::dumpToFile()
{
	return dumpToFile("logs\\" + Time::g_Time->getFormattedDateTime() + ".log");
}