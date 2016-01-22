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
	_input = new System::InputDevice(_window->GetHWND());
	_controls = new System::Controls(_input);
	_fontWrapper = new FontWrapper(_renderModule->GetDevice(), L"Assets/Fonts/Calibri.ttf", L"Calibri");

	//Init camera
	_camera = new System::Camera(0.1f, 1000.0f, DirectX::XM_PIDIV2, settings._width, settings._height);
	_camera->SetPosition(XMFLOAT3(3, 10, 0));
	_camera->SetRotation(XMFLOAT3(60, 0, 0));
	_camera->SetMode(System::FREE_CAM);
	_input->ToggleCursorLock();

	_timer = System::Timer();

	GameObjectInfo* data = new GameObjectInfo();
	GameObjectDataLoader gameObjectDataLoader;
	gameObjectDataLoader.WriteSampleGameObjects();
	gameObjectDataLoader.LoadGameObjectInfo(data);

	_objectHandler = new ObjectHandler(_renderModule->GetDevice(), _assetManager, data);
	_pickingDevice = new PickingDevice(_camera, _window);
	_SM = new StateMachine(_controls, _objectHandler, _input, _camera, _pickingDevice, "Assets/gui.json", _assetManager, _fontWrapper, settings._width, settings._height);

	_SM->Update(_timer.GetFrameTime());
	
	if (_SM->GetState() == LEVELEDITSTATE)
	{
		_grid = new Grid(_renderModule->GetDevice(), 1, 10);
	}
	else
	{
		_grid = nullptr;
	}

	//CheckSettings();
	_controls->SaveKeyBindings(System::MAP_EDIT_KEYMAP, "MOVE_CAMERA_UP", "M");

	Renderer::Spotlight* spot;
	for (int i = 0; i < 2; i++)
	{
		spot = new Renderer::Spotlight(_renderModule->GetDevice(), 0.1f, 1000.0f, XM_PIDIV4 /*XM_PI / 0.082673f*/, Renderer::SHADOWMAP_DIMENSIONS, Renderer::SHADOWMAP_DIMENSIONS, 1.0f, 8.0f, XMFLOAT3(0.0f, 1.0f, 1.0f), 36); //Ska ha samma dimensions som shadow map, som nu ligger i render module
		spot->SetPositionAndRotation(XMFLOAT3(3*i+5, 1.0f, 2*i+4), XMFLOAT3(0, 90 + i*25, 0));
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
	SAFE_DELETE(_window);
	SAFE_DELETE(_renderModule);
	SAFE_DELETE(_camera);
	SAFE_DELETE(_objectHandler);
	SAFE_DELETE(_SM);
	SAFE_DELETE(_controls);
	SAFE_DELETE(_assetManager);
	SAFE_DELETE(_pickingDevice);
	SAFE_DELETE(_input);
	SAFE_DELETE(_grid);
	delete _fontWrapper;

	for(auto s : _spotlights)
	{
		delete s;
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
	//_input->Update();
	_SM->Update(deltaTime);
	_objectHandler->Update(deltaTime);

	std::vector<GameObject*> e = _objectHandler->GetAllByType(GUARD);

	//if (e.size() > 0)
	//{
	//	XMFLOAT3 p = e[0]->GetPosition();
	//	XMFLOAT3 r = e[0]->GetRotation();
	//	XMFLOAT3 d = _spotlights[0]->GetDirection();

	//	p.y += 0.15f;
	//	p.x += d.x * 0.5f;
	//	p.z += d.z * 0.5f;

	//	r = XMFLOAT3(XMConvertToDegrees(r.x), -XMConvertToDegrees(r.y), XMConvertToDegrees(r.z));
	//	_spotlights[0]->SetPositionAndRotation(p, r);
	//}

	for (unsigned int i = 0; i < _spotlights.size(); i++)
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
	_renderModule->BeginScene(0.0f, 0.5f, 0.5f, 1.0f);
	_renderModule->SetDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());
	/*--------------------------------------------------------- Geometry pass ------------------------------------------------------------
	Render the objects to the diffuse and normal resource views. Camera depth is also generated here.									*/

	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::GEO_PASS);

	//Render all objects to the diffuse and normal buffers
	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (GameObject* g : gameObjects->at(i))
		{
			//TODO: If type == ENEMY -> don't render. Instead check in the chosen objects for the light rendering (Functionality not done), and check if the enemies are there they should be both rendered and lit.
			//If not, they should not be rendered nor lit /Jonas
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

	/*------------------------------------------------------------ Light pass --------------------------------------------------------------
	Generate the shadow map for each spotlight, then apply the lighting/shadowing to the backbuffer render target with additive blending. */

	//for (int i = 0; i < 2; i++)
	//{
	//	_renderModule->DEBUG_RenderLightVolume(_spotlights[i]->GetVolumeBuffer(), _spotlights[i]->GetWorldMatrix(), _spotlights[i]->GetVertexCount(), _spotlights[i]->GetVertexSize());
	//}

	_renderModule->SetLightDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());
	for (int i = 0; i < 2; i++)
	{
		_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::SHADOW_GENERATION);
		_renderModule->SetShadowMapDataPerSpotlight(_spotlights[i]->GetViewMatrix(), _spotlights[i]->GetProjectionMatrix());

		for (int i = 0; i < NR_OF_TYPES; i++)
		{
			for (GameObject* g : gameObjects->at(i))
			{
				_renderModule->RenderShadowMap(g->GetMatrix(), g->GetRenderObject());
			}
		}

		_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::LIGHT_APPLICATION);
		_renderModule->SetLightDataPerSpotlight(_spotlights[i]);

		//_renderModule->DEBUG_RenderLightVolume(_spotlights[i]->GetVolumeBuffer(), _spotlights[i]->GetWorldMatrix(), _spotlights[i]->GetVertexCount(), _spotlights[i]->GetVertexSize());

		_renderModule->RenderScreenQuad();
	}

	/*-------------------------------------------------------- HUD and other 2D -----------------------------------------------------------*/

	//_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::HUD_PASS);

	//_renderModule->Render(_UI->GetTextureData());
	_renderModule->Render(_SM->GetCurrentStatePointer()->GetUITree()->GetRootNode(), _fontWrapper);
	_renderModule->EndScene();
}

int Game::Run()
{
	while (_window->Run())
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
		_gameHandle->_input->HandleRawInput(lparam);
	}

	default:
	{
		return _gameHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}