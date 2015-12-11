#pragma once

#include "stdafx.h"

#include "InitVar.h"
#include "BaseState.h"
#include "SplashState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "OptionsState.h"
#include <vector>

class StateMachine
{
private:
	State					_currentState;
	std::vector<BaseState*>	_baseStates;
	void ProcessStateRequest();

public:
	StateMachine(InitVar initVar);
	~StateMachine();

	void Update(float deltaTime);
};