#ifndef SPLASHSTATE_H
#define SPLASHSTATE_H

#include "BaseState.h"

__declspec(align(16))class SplashState : public BaseState
{
private:
public:
	SplashState(InitVar initVar);
	~SplashState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};

#endif