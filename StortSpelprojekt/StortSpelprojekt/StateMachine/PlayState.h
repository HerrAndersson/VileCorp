#pragma once

#include "BaseState.h"
#include "../GameLogic.h"

class PlayState : public BaseState
{
private:
	GameLogic _gameLogic;
	InitVar _initVar;
public:
	PlayState(InitVar initVar);
	~PlayState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};