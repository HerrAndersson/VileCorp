#pragma once

#include "stdafx.h"

#include "BaseState.h"
#include "SplashState.h"
#include "MenuState.h"
#include "PlayState.h"
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
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputDevice;
	System::Camera*			_camera;
	PickingDevice*			_pickingDevice;

public:
	StateMachine(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice);
	~StateMachine();

	void Update(float deltaTime);
	State GetState();
};