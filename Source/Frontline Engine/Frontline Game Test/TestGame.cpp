#include "TestGame.h"
#include <Process\ProcessManager.h>
#include <Process\TimerProcess.h>
#include "TestPhysicsProcess.h"
#include "LockFramerateProcess.h"
#include <iostream>

TestGame::TestGame()
{
	mp_eventManager = FL_NEW EventManager();
	mp_actorFactory = ActorFactory::g_ActorFactory;
	mp_log = FL_NEW Log();
	mp_time = FL_NEW Time();
	mp_processManager = FL_NEW ProcessManager();
	window = FL_NEW WindowManager();
	mp_physics = std::shared_ptr<IPhysics>(FL_NEW BulletPhysics());
	mp_physics->VOnInit();
}


TestGame::~TestGame()
{

}


void TestGame::CollisionEventCallback(IEventDataPtr pp_event)
{
	CollisionPairEventData lp_cast = *((CollisionPairEventData*) (pp_event.get()));
	std::string addremovetext = (lp_cast.m_addOrRemove) ? "Added" : "Removed";
	char* A1TXT = FL_NEW char(), *A2TXT = FL_NEW char();
	_itoa(lp_cast.m_actor1, A1TXT, 10);
	_itoa(lp_cast.m_actor2, A2TXT, 10);
	mp_log->logOther("PHYSICS", addremovetext + " collision between actors " + std::string(A1TXT) + " and " + std::string(A2TXT));
}

void TestGame::KeyboardCallback(IEventDataPtr pp_event)
{
	KeyboardEventData lp_cast = *((KeyboardEventData*) (pp_event.get()));
	char* downuptext = (lp_cast.m_down) ? "down" : "up";
	char* codeTXT = FL_NEW char();
	//_itoa(lp_cast.code, codeTXT, 10);
	mp_log->logOther("Keyboard", "Key " + std::string(codeTXT) + " is " + downuptext);
	std::shared_ptr<TimerProcess> lp_processPtr = std::shared_ptr<TimerProcess>(FL_NEW TimerProcess());
	lp_processPtr->m_countdownMS = 1000;
	mp_processManager->AttachProcess(lp_processPtr);
	mp_log->logOther("PROCESS", "Timer process started");
}

void TestGame::start()
{
	float AccumMS = 0;
	IGame::gp_game = this;
	mp_eventManager->VAddListener(fastdelegate::MakeDelegate(this, &TestGame::CollisionEventCallback), CollisionPairEventData::sk_eventType);
	mp_eventManager->VAddListener(fastdelegate::MakeDelegate(this, &TestGame::KeyboardCallback), KeyboardEventData::sk_eventType);

	ActorFactory::g_ActorFactory->RegisterComponentType<PhysicsComponent>("PhysicsComponent");
	ActorFactory::g_ActorFactory->RegisterComponentType<TransformComponent>("TransformComponent");
	zrf = FL_NEW ZipResourceFile();
	zrf->VOpen(L"testzip.zip");

	int size;
	
	// The first actor is a small sphere
	size = zrf->VGetRawResourceSize("actorsrc1.xml");
	char* RawBuffer = FL_NEW char[size];
	zrf->VGetResource(&RawBuffer, "actorsrc1.xml");
	StrongActorPtr sphere = ActorFactory::g_ActorFactory->CreateActor(RawBuffer);
	delete RawBuffer;

	// The second actor is a larger sphere below the first sphere
	size = zrf->VGetRawResourceSize("actorsrc2.xml");
	char* RawBuffer2 = FL_NEW char[size];
	zrf->VGetResource(&RawBuffer2, "actorsrc2.xml");
	StrongActorPtr sphere2 = ActorFactory::g_ActorFactory->CreateActor(RawBuffer2);
	delete RawBuffer2;
	
	// The third actor is a box that serves as the ground
	size = zrf->VGetRawResourceSize("actorsrc3.xml");
	char* RawBuffer3 = FL_NEW char[size];
	zrf->VGetResource(&RawBuffer3, "actorsrc3.xml");
	StrongActorPtr box = ActorFactory::g_ActorFactory->CreateActor(RawBuffer3);
	delete RawBuffer3;

	// Open the game window
	window->open(800, 600, "Frontline", false, false);

	// Set up OpenGL
	window->createGLContext();

	// Select old OpenGL settings (for diagnostic rendering)
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 800.0 / 600.0, .01, 10000);
	glDepthFunc(GL_LESS);

	// This function needs to be called to provide a baseline for when the game starts
	IGame::gp_game->mp_time->ticksSinceLastCall();

	// Set up the processes
	StrongProcessPtr phys = StrongProcessPtr(new TestPhysicsProcess(mp_physics));
	mp_processManager->AttachProcess(phys);
	StrongProcessPtr frameLock = StrongProcessPtr(new LockFramerateProcess(60));
	mp_processManager->AttachProcess(frameLock);
	while (window->isOpen())
	{
		long ll_deltaMS = IGame::gp_game->mp_time->ticksSinceLastCall();

		// Deal with events
		mp_eventManager->VUpdate(10000);

		// Update the running processes - Physics, Framerate lock, etc
		mp_processManager->UpdateProcesses(ll_deltaMS);

		// Clear the screen for rendering
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Reset the OpenGL matrices
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(10, 10, 10, 0, 0, 0, 0, 1, 0);

		// Sync the physics with the actors
		mp_physics->VSyncVisibleScene();

		// Render the scene
		mp_physics->VRenderDiagnostics();
		window->swapGLBuffers();
		window->updateWindow();
	}
}

int main()
{
	// Clear the default empty game object
	delete IGame::gp_game;

	// Create the memory pool
	Pool* p = new Pool(1024, malloc(1024 * 1024));

	// Initialize the game
	TestGame* test = FL_POOL_NEW(p, TestGame) TestGame();
	test->alloc = p;

	// Start the game. This function is blocking, meaning that the next line of code will not run until
	// the game has been ended.
	test->start();

	// When the game has been exited, capture the logs
	std::vector<std::pair<int, std::string>> log = IGame::gp_game->mp_log->dumpLog();

	// Print the log to the console
	std::string ls_text;
	for (int i = 0; i < log.size(); i++)
	{
		ls_text.append(log[i].second);
		ls_text.append("\n");
	}


	std::cout << ls_text;
	destroy<IGame>(test, p);
	system("pause");
	FL_POOL_DELETE(test, p);
}