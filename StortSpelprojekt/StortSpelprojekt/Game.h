#pragma once

#include "stdafx.h"
#include "StateMachine.h"
#include "Window.h"
#include "RenderModule.h"
#include "Camera.h"
#include "AssetManager.h"

class Game
{
private:

	StateMachine				_SM;
	System::Window*				_window;
	Renderer::RenderModule*		_renderModule;
	System::Camera*				_camera;

	//Resizing window, directx resources, camera
	void ResizeResources(System::WindowSettings settings);

public:

	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	int Run();
};