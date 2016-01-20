#pragma once

#include "BaseState.h"

class SplashState : public BaseState
{
private:
public:
	SplashState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice);
	virtual ~SplashState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};