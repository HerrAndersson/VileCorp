#pragma once

#include "BaseState.h"

class MenuState : public BaseState
{
private:


public:
	MenuState(System::Controls* controls, ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	virtual ~MenuState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};
