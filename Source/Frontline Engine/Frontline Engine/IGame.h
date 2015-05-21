#pragma once
#include <memory>

class ProcessManager;
class EventManager;
class ActorFactory;
class Log;
class Time;
class IPhysics;
class Pool;

class IGame
{
public:
	EventManager* mp_eventManager;
	ActorFactory* mp_actorFactory;
	Log* mp_log;
	Time* mp_time;
	ProcessManager* mp_processManager;
	Pool* mp_defaultPool;
	std::shared_ptr<IPhysics> mp_physics;
	static IGame* gp_game;
	virtual void start();
	virtual ~IGame() {};
};