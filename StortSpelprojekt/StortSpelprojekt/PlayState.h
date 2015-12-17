#pragma once

#include "BaseState.h"

class PlayState : public BaseState
{
private:
public:
	PlayState(InitVar initVar);
	~PlayState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};