#pragma once

#include "stdafx.h"
#include "StateMachine.h"
#include "Window.h"
#include "RenderModule.h"
#include "Camera.h"
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "Timer.h"
#include "AssetManager.h"
#include "InitVar.h"
#include "Trap.h"
#include "InputDevice.h"
#include "PickingDevice.h"
#include "Player.h"



class Game
{
private:
	const float	MS_PER_FRAME = 1000 / 60;

	StateMachine*				_SM;
	System::Window*				_window;
	Renderer::RenderModule*		_renderModule;
	System::Camera*				_camera;
	ObjectHandler*				_objectHandler;
	UIHandler*					_UI;
	System::Timer				_timer;

	System::InputDevice*		_input;
	AssetManager*				_assetManager;
	PickingDevice*				_pickingDevice;
	Player*						_player;
	

	//Resizing window, directx resources, camera
	void ResizeResources(System::WindowSettings settings);
	void HandleInput();
	//Returns picked objects
	void Update(float deltaTime);
	void Render();

public:

	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	int Run();
};