#pragma once

#include "BaseState.h"

class MenuState : public BaseState
{
private:


public:
	MenuState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice);
	virtual ~MenuState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};
