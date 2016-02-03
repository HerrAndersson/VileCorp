#pragma once

#include "stdafx.h"
#include "StateMachine/StateMachine.h"
#include "Window.h"
#include "RenderModule.h"
#include "Camera.h"
#include "ObjectHandler.h"
#include "Timer.h"
#include "AssetManager.h"
#include "Trap.h"
#include "Controls.h"
#include "Grid.h"
#include "PickingDevice.h"
#include "Spotlight.h"
#include "Pointlight.h"
#include "GameObjectDataLoader.h"
#include "ShadowMap.h"
#include "LightCulling.h"

class Game
{
private:
	const float	MS_PER_FRAME = 1000.0f / 60;

	StateMachine*				_SM;
	System::Window*				_window;
	Renderer::RenderModule*		_renderModule;
	System::Camera*				_camera;
	ObjectHandler*				_objectHandler;
	System::Timer				_timer;
//	Animation*					_animation;
	System::Controls*			_controls;
	AssetManager*				_assetManager;
	Grid*						_grid;
	FontWrapper*				_fontWrapper;
	PickingDevice*				_pickingDevice;
	PlayerInfo					_playerInfo;
	SettingInfo					_gameSettings;
	System::WindowSettings		_windowSettings;

	bool						_hasFocus;
	bool						_enemiesHasSpawned;

	//Resizing window, directx resources, camera
	void ResizeResources(const System::WindowSettings& settings);
	bool Update(float deltaTime);
	void Render();
	void CheckSettings();
	
	//TODO: TEMP! Move this to objectHandler
	std::vector<Renderer::Spotlight*> _spotlights;
	std::vector<Renderer::Pointlight*> _pointlights;
	LightCulling* _lightCulling;

	std::vector<GameObject*> _enemies;
	std::vector<GameObject*> _loot;
public:

	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	int Run();
};

static Game* _gameHandle;
