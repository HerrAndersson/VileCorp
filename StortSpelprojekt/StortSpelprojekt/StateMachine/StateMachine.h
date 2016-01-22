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

	ObjectHandler*			_objectHandler;
	System::InputDevice*	_inputDevice;
	System::Camera*			_camera;
	PickingDevice*			_pickingDevice;

public:
	StateMachine(
		System::Controls* controls,
		ObjectHandler* objectHandler,
		System::InputDevice* inputDevice,
		System::Camera* camera,
		PickingDevice* pickingDevice,
		const std::string& filename,
		AssetManager* assetManager,
		FontWrapper* fontWrapper,
		int width,
		int height);
	~StateMachine();

	void virtual Update(float deltaTime);
	State GetState();
	BaseState* GetCurrentStatePointer() const;
	void Resize(int width, int height);
};