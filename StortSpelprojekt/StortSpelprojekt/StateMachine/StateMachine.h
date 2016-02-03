#pragma once

#include "stdafx.h"

#include "BaseState.h"
#include "SplashState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "PlacementState.h"
#include "LevelEditState.h"
#include "OptionsState.h"
#include <vector>

class StateMachine
{
private:
	State					_currentState;
	std::vector<BaseState*>	_baseStates;
	void ProcessStateRequest();

public:
	StateMachine(
		System::Controls* controls,
		ObjectHandler* objectHandler,
		System::Camera* camera,
		PickingDevice* pickingDevice,
		const std::string& filename,
		AssetManager* assetManager,
		FontWrapper* fontWrapper,
		int width,
		int height);
	~StateMachine();

	bool Update(float deltaTime);
	State GetState();
	BaseState* GetCurrentStatePointer() const;
	void Resize(int width, int height);
};