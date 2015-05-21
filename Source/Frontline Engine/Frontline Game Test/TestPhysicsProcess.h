#pragma once
#include <Process\Process.h>
#include <Physics\BulletPhysics.h>
class TestPhysicsProcess :
	public Process
{
	std::shared_ptr<IPhysics> mp_physics;
	int ups;
	float accum;
protected:
	virtual void VOnUpdate(unsigned long deltaMs) override;
public:
	TestPhysicsProcess(std::shared_ptr<IPhysics> physics);
	~TestPhysicsProcess();
};

