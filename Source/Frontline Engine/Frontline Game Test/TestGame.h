#pragma once
#include <gl/GLEW.h>
#include <IGame.h>
#include <FrontlineCommon.h>
#include <Actor\ActorFactory.h>
#include <Physics\BulletPhysics.h>
#include <System\IWindowManager.h>
#include <ActorComponents.h>
#include <fstream>
#include <System\Time.h>
#include <Resource\ZipResourceFile.h>
#include <Event\AddCollisionPairEventData.h>
#include <Resource\ResCache.h>
#ifdef _WIN32
#include <System\Win32WindowManager.h>
#define WindowManager Win32WindowManager
#endif

class TestGame :
	public IGame
{
protected:
	WindowManager* window;
	ZipResourceFile* zrf;
public:
	Pool* alloc;
	TestGame();
	~TestGame();
	virtual void start() override;
	void CollisionEventCallback(IEventDataPtr pp_event);
	void KeyboardCallback(IEventDataPtr pp_event);
};