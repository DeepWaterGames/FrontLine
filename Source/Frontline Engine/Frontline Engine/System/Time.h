#pragma once
#include "..\FrontlineCommon.h"
#include "../IGame.h"
#include <Windows.h>
#include <iomanip>
#include <ctime>
#include <sstream>

class Time
{
	int m_timer;
public:
	Time();
	~Time();
	unsigned long m_startMs;
	unsigned long sysTimeMs();
	unsigned long timeElapsedSinceMS(long time);
	unsigned long getSysTimeS();
	void sleep(long sleepMS);
	inline unsigned long ticksSinceLastCall()
	{
		long l_retval = timeElapsedSinceMS(m_timer);
		m_timer = sysTimeMs();
		return l_retval;
	}
	std::string getFormattedDateTime();
	static Time* g_Time;
};

