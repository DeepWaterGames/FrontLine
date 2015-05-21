#pragma once
#include "System\Pool.h"
#define DEFAULT_POOL IGame::gp_game->mp_defaultPool
#include "IGame.h"

#include <string>
#include <map>
#include <algorithm>
#include "Math\Math.h"
#include "Templates.h"
#include "System\Log.h"
#include "System\Time.h"
#include "Actor\Actor.h"
#include "Event\EventManager.h"
#include <GLM/ext.hpp>
#include "flstringutils.h"
#include <intrin.h>

#ifdef _DEBUG
#define ERRLOG(Category, Message) IGame::gp_game->mp_log->logError(Category, Message); __debugbreak()
#else
#define ERRLOG IGame::gp_game->mp_log->logError
#endif

#define WARNLOG IGame::gp_game->mp_log->logWarning
#define MSGLOG IGame::gp_game->mp_log->logOther

#define FL_ASSERT assert
#define FL_NEW new
#define FL_DELETE delete

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif