#pragma once

#include "stdafx.h"
#include "StateMachine.h"
#include "Window.h"
#include "RenderModule.h"
#include "Camera.h"
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "AssetManager.h"
#include "InitVar.h"
#include "Trap.h"
#include "InputDevice.h"

class Game
{
private:
	StateMachine*				_SM;
	System::Window*				_window;
	Renderer::RenderModule*		_renderModule;
	System::Camera*				_camera;
	ObjectHandler*				_objectHandler;
	UIHandler*					_UI;

	System::InputDevice*		_input;

	//Resizing window, directx resources, camera
	void ResizeResources(System::WindowSettings settings);
	void HandleInput();
	void Update(float deltaTime);
	void Render();

public:

	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	int Run();
};