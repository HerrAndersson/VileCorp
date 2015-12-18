#pragma once
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "InputDevice.h"
#include "StateMachine/InitVar.h"

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
	void Initialize(InitVar* initVar);
	void Update(float deltaTime);
};