#pragma once

#include "StateMachine/StateMachine.h"
#include "SoundModule.h"
#include "stdafx.h"
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
#include "ShadowMap.h"
#include "Particle system\ParticleHandler.h"
#include "Particle system\ParticleUtils.h"
#include "SettingsReader.h"

class Game
{
private:
	const float	MS_PER_FRAME = 1000.0f / 60;

	StateMachine*				_SM;
	System::Window*				_window;
	Renderer::RenderModule*		_renderModule;
	Renderer::ParticleHandler*  _particleHandler;
	System::Camera*				_camera;
	ObjectHandler*				_objectHandler;
	System::Timer				_timer;
//	Animation*					_animation;
	System::Controls*			_controls;
	AssetManager*				_assetManager;
	FontWrapper*				_fontWrapper;
	PickingDevice*				_pickingDevice;
	PlayerInfo					_playerInfo;
	SettingInfo					_gameSettings;
	System::SettingsReader		_settingsReader;
	GameObjectInfo				_data;
	System::SoundModule			_soundModule;

	bool						_hasFocus;
	bool						_enemiesHasSpawned;

	//Resizing window, directx resources, camera
	void ResizeResources(System::Settings* settings);
	bool Update(double deltaTime);
	void Render();

	std::vector<GameObject*> _enemies;
	std::vector<GameObject*> _loot;

public:

	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

	int Run();
};

static Game* _gameHandle;
