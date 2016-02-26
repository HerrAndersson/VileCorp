#pragma once

#include "BaseState.h"
#include "SplashState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "PlacementState.h"
#include "LevelEditState.h"
#include "OptionsState.h"
#include "LevelSelectState.h"
#include "PauseState.h"
#include <vector>
#include "stdafx.h"
#include "../CombinedMeshGenerator.h"

class StateMachine
{
private:
	State					_currentState;
	std::vector<BaseState*>	_baseStates;
	void ProcessStateRequest();

public:

	StateMachine
	(
		System::Controls* controls,
		ObjectHandler* objectHandler,
		System::Camera* camera,
		PickingDevice* pickingDevice,
		const std::string& filename,
		AssetManager* assetManager,
		FontWrapper* fontWrapper,
		System::Settings* settings,
		System::SettingsReader* settingsReader,
		System::SoundModule* soundModule,
		DirectX::XMFLOAT3 *ambientLight,
		CombinedMeshGenerator* combinedMeshGenerator
	);

	~StateMachine();

	bool Update(float deltaTime);
	State GetState();
	BaseState* GetCurrentStatePointer() const;
	void Resize(System::Settings* settings);
};