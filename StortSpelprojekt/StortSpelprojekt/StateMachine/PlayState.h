#pragma once

#include "BaseState.h"
#include "../GameLogic.h"

class PlayState : public BaseState
{
private:
	GameLogic _gameLogic;
public:
	PlayState(System::Controls* controls, ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	virtual ~PlayState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};