#include "Time.h"

Time* Time::g_Time = FL_NEW Time();

Time::Time()
{
	m_startMs = GetTickCount64();
	m_timer = 0;
}


Time::~Time()
{
}

unsigned long Time::sysTimeMs()
{
	return GetTickCount64();
}

unsigned long Time::timeElapsedSinceMS(long p_timeSince)
{
	return GetTickCount64() - p_timeSince;
}

unsigned long Time::getSysTimeS()
{
	return (unsigned long) time(0);
}

std::string Time::getFormattedDateTime()
{
	std::time_t l_time = std::time(nullptr);
	std::tm l_tm;
	localtime_s(&l_tm, &l_time);
	std::stringstream l_timeBuffer;
	l_timeBuffer << std::put_time(&l_tm, "%b-%d-%Y %H.%M.%S");
	return l_timeBuffer.str();
}

void sleep(long sleepMS)
{
	Sleep(sleepMS);
}