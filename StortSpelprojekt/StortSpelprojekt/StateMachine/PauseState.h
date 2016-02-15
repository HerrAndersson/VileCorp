#pragma once

#include "BaseState.h"

class PauseState : public BaseState
{
public:
	PauseState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule);
	virtual ~PauseState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};