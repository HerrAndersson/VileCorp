#pragma once
#include <DirectXMath.h>
#include "..\System\SoundModule.h"
#include "ObjectHandler.h"
#include "Camera.h"
#include "Controls.h"
#include "PickingDevice.h"
#include "Player.h"
#include "JsonStructs.h"
#include "..\System\JsonParser.h"
#include "UITree.h"
#include "AssetManager.h"
#include "InputDevice.h"

class GameLogic
{
private:
	ObjectHandler*			_objectHandler;
	System::Camera*			_camera;
	System::Controls*		_controls;
	PickingDevice*			_pickingDevice;
	Player*					_player;
	GUI::UITree*			_uiTree;
	AssetManager*			_assetManager;
	Texture*				_guardTexture;
	bool					_returnToMenu;
	System::SettingsReader* _settingsReader;
	float					_buttonReady;
	bool					_gameOver;
	System::SoundModule*	_soundModule;
	int						_nrOfLoot;

	int _gameMode;
	int _surviveForSeconds;

	void HandleInput(float deltaTime);
	void HandleUnitSelect();
	void PlaySelectSound(GuardType guardType);
	void ShowSelectedInfo();
	void HandlePatrol();
	void HandleUnitMove();
	void PlayMoveSound(GuardType guardType);
	void HandleWinLoseDialog(float deltaTime);
	bool CheckGameStatus();
public:
	GameLogic(ObjectHandler* objectHandler, System::Camera* camera, System::Controls* controls, PickingDevice* pickingDevice, GUI::UITree* uiTree, AssetManager* assetManager, System::SettingsReader* settingsReader, System::SoundModule* soundModule);
	~GameLogic();
	void Update(float deltaTime);
	bool GoToMenu()const;
	void SetNrOfLoot(int nrOfLoot);
};



