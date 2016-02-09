#pragma once

#include "BaseState.h"
#include "../GameLogic.h"

class PlayState : public BaseState
{
private:
	GameLogic _gameLogic;
public:
	PlayState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule);
	virtual ~PlayState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};