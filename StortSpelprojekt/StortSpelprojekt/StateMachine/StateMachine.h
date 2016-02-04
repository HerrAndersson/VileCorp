#pragma once

#include "stdafx.h"

#include "BaseState.h"
#include "SplashState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "PlacementState.h"
#include "LevelEditState.h"
#include "OptionsState.h"
#include "LevelSelectState.h"
#include <vector>

class StateMachine
{
private:
	State					_currentState;
	std::vector<BaseState*>	_baseStates;
	void ProcessStateRequest();

	ObjectHandler*			_objectHandler;
	System::Camera*			_camera;
	PickingDevice*			_pickingDevice;

public:
	StateMachine(
		System::Controls* controls,
		ObjectHandler* objectHandler,
		System::Camera* camera,
		PickingDevice* pickingDevice,
		const std::string& filename,
		AssetManager* assetManager,
		FontWrapper* fontWrapper,
		System::Settings* settings,
		System::SettingsReader* settingsReader);
	~StateMachine();

	bool Update(float deltaTime);
	State GetState();
	BaseState* GetCurrentStatePointer() const;
	void Resize(System::Settings* settings);
};