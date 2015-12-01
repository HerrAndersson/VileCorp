#pragma once

#include "stdafx.h"
#include "StateMachine.h"
#include "Window.h"
#include "RenderModule.h"
#include "Camera.h"
#include "AssetManager.h"
#include "Trap.h"
#include "ObjectHandler.h"

class Game
{
private:

	StateMachine				_SM;
	System::Window*				_window;
	Renderer::RenderModule*		_renderModule;
	System::Camera*				_camera;
	ObjectHandler*				_objectHandler;

	//Temp //Mattias
	AssetManager* _tempAssetManager;
	RenderObject* _tempTestRenderObject;
	float stepValue;

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