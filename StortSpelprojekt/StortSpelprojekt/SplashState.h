#pragma once

#include "BaseState.h"

class SplashState : public BaseState
{
private:
public:
	SplashState(InitVar initVar);
	~SplashState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};