#pragma once


#include "stdafx.h"
#include "StateMachine.h"
#include "Window.h"
#include "RenderModule.h"
#include "Camera.h"
#include "AssetManager.h"
#include "Trap.h"
#include "Timer.h"
#include "ObjectHandler.h"

class Game
{
private:
	const float MS_PER_FRAME = 1000/60;

	StateMachine				_SM;
	System::Window*				_window;
	Renderer::RenderModule*			_renderModule;
	System::Camera*				_camera;
	ObjectHandler*				_objectHandler;

	System::Timer timer;

	//Resizing window, directx resources, camera
	void ResizeResources(System::WindowSettings settings);
	void HandleInput();
	void Update();
	void Render();

public:

	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	int Run();
};