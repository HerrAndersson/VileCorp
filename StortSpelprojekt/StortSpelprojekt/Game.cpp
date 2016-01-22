#include "Game.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

Game::Game(HINSTANCE hInstance, int nCmdShow)
{
	_gameHandle = this;
	System::WindowSettings settings;
	_window = new System::Window("Amazing game", hInstance, settings, WndProc);

	_timer = System::Timer();

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings._width, settings._height);
	
	_assetManager = new AssetManager(_renderModule->GetDevice());
	_controls = new System::Controls(_window->GetHWND());
	_fontWrapper = new FontWrapper(_renderModule->GetDevice(), L"Assets/Fonts/Calibri.ttf", L"Calibri");

	//Init camera
	_camera = new System::Camera(0.1f, 1000.0f, DirectX::XM_PIDIV2, settings._width, settings._height);
	_camera->SetPosition(XMFLOAT3(3, 10, 0));
	_camera->SetRotation(XMFLOAT3(60, 0, 0));

	_timer = System::Timer();


	GameObjectInfo* data = new GameObjectInfo();
	GameObjectDataLoader gameObjectDataLoader;
	gameObjectDataLoader.WriteSampleGameObjects();
	gameObjectDataLoader.LoadGameObjectInfo(data);
	_objectHandler = new ObjectHandler(_renderModule->GetDevice(), _assetManager, data);
	//Init statemachine
	//_SM = new StateMachine();
	//Init statemachine
	_pickingDevice = new PickingDevice(_camera, _window);
	_SM = new StateMachine(_controls, _objectHandler, _camera, _pickingDevice, "Assets/gui.json", _assetManager, _fontWrapper, settings._width, settings._height);

	_SM->Update(_timer.GetFrameTime());
	if (_SM->GetState() == LEVELEDITSTATE)
	{
		_grid = new Grid(_renderModule->GetDevice(), 1, 10);
	}

	//CheckSettings();
	_controls->SaveKeyBindings(System::MAP_EDIT_KEYMAP, "MOVE_CAMERA_UP", "M");

	//TODO: TEMP! Make this pretty
	Renderer::Spotlight* spot;
	for (int i = 0; i < 4; i++)
	{
		spot = new Renderer::Spotlight(_renderModule->GetDevice(), 0.1f, 1000.0f, XM_PIDIV4 /*XM_PI / 0.082673f*/, 256, 256, 1.0f, 10.0f, XMFLOAT3(0.0f, 1.0f, 1.0f), 36); //Ska ha samma dimensions som shadow map, som nu ligger i render module
		spot->SetPositionAndRotation(XMFLOAT3(4 * i + 3, 1.5f, 3 * i + 3), XMFLOAT3(0, 90 + i * 25, 0));
		_spotlights.push_back(spot);
	}
}

void Game::CheckSettings()
{
	////System::saveJSON(&_gameSettings, "Assets/GameSettings.json", "Game Settings");
	//System::loadJSON(&_gameSettings, "Assets/GameSettings.json");

	//if (_gameSettings._default == false)
	//{
	//	System::WindowSettings winSettings = _window->GetWindowSettings();
	//	winSettings._width = _gameSettings._resX;
	//	winSettings._height = _gameSettings._resY;
	//	winSettings._flags = 0;
	//	if (_gameSettings._fullScreen == true)
	//	{
	//		winSettings._flags += 1;
	//	}
	//	else if (_gameSettings._bordeless == true)
	//	{
	//		winSettings._flags += 2;
	//	}
	//	else if (_gameSettings._showMouseCursor == true)
	//	{
	//		winSettings._flags += 4;
	//	}
	//	_window->ResizeWindow(winSettings);
	//}
}

Game::~Game() 
{
	delete _window;
	delete _renderModule;
	delete _camera;
	delete _objectHandler;
	delete _SM;
	delete _controls;
	delete _assetManager;	
	delete _pickingDevice;
	delete _fontWrapper;

	
	//TODO: TEMP! dfhfa
	for(auto s : _spotlights)
	{
		delete s;
	}

	if (_grid != nullptr)
	{
		delete _grid;
	}
}

void Game::ResizeResources(System::WindowSettings settings)
{
	_window->ResizeWindow(settings);
	_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
	_camera->Resize(settings._width, settings._height);
	_SM->Resize(settings._width, settings._height);
}



void Game::Update(float deltaTime)
{
	/*
	Object handler update

	Fetch from objecthandler or different update functions in objecthandler

	*/
	_controls->Update();
	_SM->Update(deltaTime);
	_objectHandler->Update(deltaTime);

	for (int i = 0; i < _spotlights.size(); i++)
	{
		XMFLOAT3 rot = _spotlights[i]->GetRotation();
		rot.y -= (float)(i + 1) / 1;
		_spotlights[i]->SetRotation(rot);

		XMFLOAT3 color = _spotlights[i]->GetColor();
		color.x = sin(_timer.GetGameTime() / 1000);
		color.y = sin(_timer.GetGameTime() / 1000 + XMConvertToRadians(120));
		color.z = sin(_timer.GetGameTime() / 1000 + XMConvertToRadians(240));
		_spotlights[i]->SetColor(color);
	}
}

void Game::Render()
{
	_renderModule->BeginScene(0.0f, 1.0f, 1.0f, 1);
	_renderModule->SetDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());

	/*/////////////////////////////////////////////////////////// Geometry pass //////////////////////////////////////////////////////////
	
	Render the objects to the diffuse and normal resource views. Camera depth is also generated here.
			
	*/
	_renderModule->SetShaderStage(Renderer::RenderModule::GEO_PASS);
	
	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (GameObject* g : gameObjects->at(i))
		{
			_renderModule->Render(g->GetMatrix(), g->GetRenderObject());
		}
	}

	if (_SM->GetState() == LEVELEDITSTATE)
	{
		_renderModule->SetShaderStage(Renderer::RenderModule::GRID_PASS);

		std::vector<DirectX::XMMATRIX>* gridMatrices = _grid->GetGridMatrices();

		for (auto &matrix : *gridMatrices)


		{
			_renderModule->RenderLineList(&matrix, _grid->GetLineBuffer(), 2);
		}
	}
//for (auto spot : _spotlights)
	//{
	//	_renderModule->DEBUG_RenderLightVolume(spot->GetVolumeBuffer(), spot->GetWorldMatrix());
	//}

	/*/////////////////////////////////////////////////////////// Light pass /////////////////////////////////////////////////////////////

	Generate the shadow map for each spotlight, then apply the lighting/shadowing to the backbuffer render target with additive blending.
	Instead of rendering the whole screen quad each time, the geometry pass should also output to the backbuffer directly,
	then the light should be applied with additive blending.

	*/

	_renderModule->SetLightDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());
	//for (auto spot : _spotlights)
	//{

		//for (int i = 0; i < 4; i++)
		//{
			_renderModule->SetShaderStage(Renderer::RenderModule::SHADOW_GENERATION);
		//	_renderModule->SetShadowMapDataPerSpotLight(_spotlights[i]->GetViewMatrix(), _spotlights[i]->GetProjectionMatrix());

		//	for (auto i : *gameObjects)
		//	{
		//		_renderModule->RenderShadowMap(i->GetMatrix(), i->GetRenderObject());
		//	}

			_renderModule->SetShaderStage(Renderer::RenderModule::LIGHT_APPLICATION);
		//	_renderModule->SetLightDataPerLight(_spotlights[i]);

			//Render light volume here instead? Render screen quad once first to fill the screen, or just let Geo pass output to backbuffer directly?

		//}
	//}

	_renderModule->RenderScreenQuad();

	_renderModule->SetShaderStage(Renderer::RenderModule::HUD_PASS);
	_renderModule->Render(_SM->GetCurrentStatePointer()->GetUITree()->GetRootNode(), _fontWrapper);
	_renderModule->EndScene();
}

int Game::Run()
{
	bool run = true;

	// Setting up delta time
		__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
	//

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (run)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				run = false;
			}
		}
		else
		{
			_timer.Update();
			if (_timer.GetFrameTime() >= MS_PER_FRAME)
			{
				Update(_timer.GetFrameTime());
				Render();
				string s = to_string(_timer.GetFrameTime()) + " " + to_string(_timer.GetFPS());

				SetWindowText(_window->GetHWND(), s.c_str());

				_timer.Reset();
			}
		}
	}

	return 0;
}

LRESULT CALLBACK Game::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


LRESULT CALLBACK Game::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_QUIT:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_INPUT:
	{
		_gameHandle->_controls->HandleRawInput(lparam);
	}

	default:
	{
		return _gameHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}