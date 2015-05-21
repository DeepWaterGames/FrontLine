#include "TestPhysicsProcess.h"


TestPhysicsProcess::TestPhysicsProcess(std::shared_ptr<IPhysics> physics)
{
	mp_physics = physics;
	ups = 120;
	accum = 0;
}

void TestPhysicsProcess::VOnUpdate(unsigned long deltaMs)
{
	accum += deltaMs;
	while (accum >= 1000.0F / ups)
	{
		mp_physics->VOnUpdate(1.0F / ups);
		accum -= 1000.0F / ups;
	}
}

TestPhysicsProcess::~TestPhysicsProcess()
{
}
