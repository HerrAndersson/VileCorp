#pragma once

#include "BaseState.h"
#include "../GameLogic.h"

class PlayState : public BaseState
{
private:
	GameLogic _gameLogic;
public:
	PlayState(System::Controls* controls, ObjectHandler* objectHandler, UIHandler* uiHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice);
	~PlayState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};