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
#include "ParticleSystem\ParticleHandler.h"
#include "ParticleSystem\ParticleUtils.h"
#include "SettingsReader.h"
#include "CombinedMeshGenerator.h"
#include "AmbientLight.h"

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
	System::Controls*			_controls;
	AssetManager*				_assetManager;
	FontWrapper*				_fontWrapper;
	PickingDevice*				_pickingDevice;
	SettingInfo					_gameSettings;
	System::SettingsReader		_settingsReader;
	GameObjectInfo				_data;
	System::SoundModule			_soundModule;
	CombinedMeshGenerator*		_combinedMeshGenerator;

	bool						_hasFocus;
	bool						_justGotFocus;
	bool						_enemiesHasSpawned;

	//Resizing window, directx resources, camera
	void ResizeResources(System::Settings* settings);

	bool Update(float deltaTime);
	void Render();

	void RenderGameObjects(int forShaderStage, std::vector<std::vector<GameObject*>>* gameObjects);
	void GenerateShadowMap(Renderer::RenderModule::ShaderStage renderStage, Renderer::Spotlight* spotlight, unsigned short ownerID);
	void RenderParticles();

	std::vector<GameObject*> _enemies;
	std::vector<GameObject*> _loot;

	void LoadParticleSystemData(ParticleTextures& particleTextures, ParticleModifierOffsets& modifiers);

	AmbientLight				_ambientLight;
	std::vector<Renderer::Spotlight*> spotlights;

public:

	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

	int Run();
};

static Game* _gameHandle;
