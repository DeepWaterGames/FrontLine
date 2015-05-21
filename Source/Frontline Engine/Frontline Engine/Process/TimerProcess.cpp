#include "TimerProcess.h"


TimerProcess::TimerProcess()
{
}


void TimerProcess::VOnUpdate(unsigned long deltaMs)
{
	m_countdownMS -= deltaMs;
	if (m_countdownMS <= 0)
	{
		Succeed();
		IGame::gp_game->mp_log->logOther("TIMERPROCESS", "Timer process finished");
	}
}