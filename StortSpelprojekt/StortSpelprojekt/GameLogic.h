#pragma once
#include "ObjectHandler.h"
#include "Camera.h"
#include "Controls.h"
#include "PickingDevice.h"
#include "Player.h"

class GameLogic
{
private:
	ObjectHandler*			_objectHandler;
	System::Camera*			_camera;
	System::Controls*		_controls;
	PickingDevice*			_pickingDevice;
	Player*					_player;

	void HandleInput();
public:
	GameLogic();
	~GameLogic();
	void Initialize(ObjectHandler* objectHandler, System::Camera* camera, System::Controls* controls, PickingDevice* pickingDevice);
	void Update(float deltaTime);
};



