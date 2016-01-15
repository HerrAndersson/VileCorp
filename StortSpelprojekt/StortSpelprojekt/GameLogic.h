#pragma once
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "InputDevice.h"
#include "Camera.h"

class GameLogic
{
private:
	ObjectHandler*			_objectHandler;
	UIHandler*				_uiHandler;
	System::InputDevice*	_inputDevice;
	System::Camera*			_camera;

	void HandleInput();
public:
	GameLogic();
	~GameLogic();
	void Initialize(System::InputDevice* inputDevice, ObjectHandler* objectHandler,  UIHandler* uiHandler, System::Camera* camera);
	void Update(float deltaTime);
};