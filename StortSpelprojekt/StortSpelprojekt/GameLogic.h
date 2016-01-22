#pragma once
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "InputDevice.h"
#include "Camera.h"
#include "Controls.h"
#include "PickingDevice.h"
#include "Player.h"

class GameLogic
{
private:
	ObjectHandler*			_objectHandler;
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputDevice;
	System::Camera*			_camera;
	System::Controls*		_controls;
	PickingDevice*			_pickingDevice;
	Player*					_player;

	void HandleInput();
public:
	GameLogic();
	~GameLogic();
	void Initialize(System::InputDevice* inputDevice, ObjectHandler* objectHandler, UIHandler* uiHandler, System::Camera* camera, System::Controls*	controls, PickingDevice* pickingDevice);
	void Update(float deltaTime);
};



