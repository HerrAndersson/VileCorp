#pragma once

#include "stdafx.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/InitVar.h"
#include "Window.h"
#include "RenderModule.h"
#include "Camera.h"
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "Timer.h"
#include "AssetManager.h"
#include "Trap.h"
#include "InputDevice.h"

class Game
{
private:
	const float	MS_PER_FRAME = 1000.0f / 60;

	StateMachine*				_SM;
	System::Window*				_window;
	Renderer::RenderModule*		_renderModule;
	System::Camera*				_camera;
	ObjectHandler*				_objectHandler;
	UIHandler*					_UI;
	System::Timer				_timer;
	System::InputDevice*		_input;
	AssetManager*				_assetManager;

	//Resizing window, directx resources, camera
	void HandleInput();
	void ResizeResources(System::WindowSettings settings);
	void Update(float deltaTime);
	void Render();

public:

	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	int Run();
};