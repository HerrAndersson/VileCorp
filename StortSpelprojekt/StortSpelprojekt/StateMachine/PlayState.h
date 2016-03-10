#pragma once

#include "BaseState.h"
#include "../GameLogic.h"
#include "../AmbientLight.h"

class PlayState : public BaseState
{
private:
	GameLogic* _gameLogic;
	AmbientLight* _ambientLight;
	int _nrOfLoot;
public:
	PlayState(System::Controls * controls, ObjectHandler * objectHandler, System::Camera * camera, PickingDevice * pickingDevice, const std::string & filename, AssetManager * assetManager, FontWrapper * fontWrapper, System::SettingsReader * settingsReader, System::SoundModule * soundModule, AmbientLight* ambientLight);
	virtual ~PlayState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};