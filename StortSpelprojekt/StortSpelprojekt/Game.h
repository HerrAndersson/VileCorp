#pragma once

#include "stdafx.h"
#include "StateMachine/StateMachine.h"
#include "Window.h"
#include "RenderModule.h"
#include "Camera.h"
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "Timer.h"
#include "AssetManager.h"
#include "Trap.h"
#include "Controls.h"
#include "Grid.h"
#include "PickingDevice.h"
#include "GameObjectDataLoader.h"

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
	Animation*					_animation;
	System::Controls*			_controls;
	AssetManager*				_assetManager;
	Grid*						_grid = nullptr;
	PickingDevice*				_pickingDevice;
	System::InputDevice*		_input;
	PlayerInfo					_playerInfo;
	
	

	//Resizing window, directx resources, camera
	void ResizeResources(System::WindowSettings settings);
	void Update(float deltaTime);
	void Render();
	

public:
	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	int Run();
};

static Game* _gameHandle;
