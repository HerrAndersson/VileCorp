#pragma once

#include "BaseState.h"

class MenuState : public BaseState
{
private:


public:
	MenuState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule);
	virtual ~MenuState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};
