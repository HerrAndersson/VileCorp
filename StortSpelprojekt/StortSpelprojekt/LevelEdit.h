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

class LevelEdit
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

	//Resizing window, directx resources, camera
	void ResizeResources(System::WindowSettings settings);
	void HandleInput();
	void Update(float deltaTime);
	void Render();

	//Level Edit
	void HandleSelected();
	void LoadLevel();
	void InitNewLevel();
	void ResetSelectedObj();

public:

	LevelEdit(HINSTANCE hInstance, int nCmdShow);
	~LevelEdit();

	//temps
	int _selectedObj;
	int _tileMultiplier;

	int _lastSelected;


	int Run();
};