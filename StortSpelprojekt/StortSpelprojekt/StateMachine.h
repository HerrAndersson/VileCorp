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
	StateMachine(InitVar initVar, const std::string & filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	~StateMachine();

	void Update(float deltaTime);
	BaseState* GetCurrentState() const;
	void Resize(int width, int height);
};