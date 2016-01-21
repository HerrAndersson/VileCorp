#include "Game.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

Game::Game(HINSTANCE hInstance, int nCmdShow)
{
	System::saveJSON(&_playerInfo, "test.json", "Player Info");
	System::loadJSON(&_playerInfo, "test.json");
	
	_gameHandle = this;
	System::WindowSettings settings;
	_window = new System::Window("Amazing game", hInstance, settings, WndProc);

	_timer = System::Timer();

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings._width, settings._height);
	
	_assetManager = new AssetManager(_renderModule->GetDevice());
	_input = new System::InputDevice(_window->GetHWND());
	_controls = new System::Controls(_input);

	//Init camera
	_camera = new System::Camera(0.1f, 1000.0f, DirectX::XM_PIDIV2, settings._width, settings._height);
	_camera->SetPosition(XMFLOAT3(3, 7, 0));
	_camera->SetRotation(XMFLOAT3(70, 0, 0));
	_camera->SetMode(System::FREE_CAM);
	_input->ToggleCursorLock();

	_UI = new UIHandler(_renderModule->GetDevice(), _window->GetWindowSettings(), _assetManager);
	
	_objectHandler = new ObjectHandler(_renderModule->GetDevice(), _assetManager);
	
	//Init statemachine
	_pickingDevice = new PickingDevice(_camera, _window);
	_SM = new StateMachine(_controls, _objectHandler, _UI, _input, _camera, _pickingDevice);
	_SM->Update(_timer.GetFrameTime());
	
	if (_SM->GetState() == LEVELEDITSTATE)
	{
		_grid = new Grid(_renderModule->GetDevice(), 1, 10);
	}
	else
	{
		_grid = nullptr;
	}

	//TODO: TEMP! Make this pretty
	Renderer::Spotlight* spot;
	for (int i = 0; i < 2; i++)
	{
		spot = new Renderer::Spotlight(_renderModule->GetDevice(), 0.1f, 1000.0f, XM_PIDIV4 /*XM_PI / 0.082673f*/, 256, 256, 1.0f, 10.0f, XMFLOAT3(0.0f, 1.0f, 1.0f), 36); //Ska ha samma dimensions som shadow map, som nu ligger i render module
		spot->SetPositionAndRotation(XMFLOAT3(3*i+5, 1.5f, 2*i+4), XMFLOAT3(0, 90 + i*25, 0));
		_spotlights.push_back(spot);
	}
}

Game::~Game() 
{
	SAFE_DELETE(_window);
	SAFE_DELETE(_renderModule);
	SAFE_DELETE(_camera);
	SAFE_DELETE(_objectHandler);
	SAFE_DELETE(_UI);
	SAFE_DELETE(_SM);
	SAFE_DELETE(_controls);
	SAFE_DELETE(_assetManager);
	SAFE_DELETE(_pickingDevice);
	SAFE_DELETE(_input);
	SAFE_DELETE(_grid);
	
	//TODO: TEMP! dfhfa
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
}

void Game::Update(float deltaTime)
{
	/*
	Object handler update

	Fetch from objecthandler or different update functions in objecthandler

	*/
	//_input->Update();
	_UI->Update();
	_UI->OnResize(_window->GetWindowSettings());
	_SM->Update(deltaTime);
	_objectHandler->Update(deltaTime);

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

	XMFLOAT3 forward(0, 0, 0);
	XMFLOAT3 position = _camera->GetPosition();
	XMFLOAT3 right(0, 0, 0);
	bool isMoving = false;
	float v = 0.1f;

	if (GetAsyncKeyState(0x57) != 0) //W
	{
		forward = _camera->GetForwardVector();
		isMoving = true;
	}
	else if (GetAsyncKeyState(0x53) != 0)//S
	{
		forward = _camera->GetForwardVector();
		forward.x *= -1;
		forward.y *= -1;
		forward.z *= -1;
		isMoving = true;
	}

	if (GetAsyncKeyState(0x44) != 0)//D
	{
		right = _camera->GetRightVector();
		isMoving = true;
	}
	else if (GetAsyncKeyState(0x41) != 0)//A
	{
		right = _camera->GetRightVector();
		right.x *= -1;
		right.y *= -1;
		right.z *= -1;
		isMoving = true;
	}

	if (isMoving)
	{
		_camera->SetPosition(XMFLOAT3(position.x + (forward.x + right.x) * v, position.y + (forward.y + right.y) * v, position.z + (forward.z + right.z) * v));
	}

	XMFLOAT3 rotation = _camera->GetRotation();
	rotation.x += _input->GetMouseCoord()._deltaPos.y / 10.0f;
	rotation.y += _input->GetMouseCoord()._deltaPos.x / 10.0f;

	_camera->SetRotation(rotation);
}

void Game::Render()
{
	_renderModule->BeginScene(0.0f, 1.0f, 1.0f, 1);
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

	//Render fullscreen quad once here! Without blending etc, just output diffuse to backbuffer. Use LightVS and LightPS

	_renderModule->SetLightDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());

	for (int i = 0; i < 2; i++)
	{
		_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::SHADOW_GENERATION);
		_renderModule->SetShadowMapDataPerSpotLight(_spotlights[i]->GetViewMatrix(), _spotlights[i]->GetProjectionMatrix());

		for (int i = 0; i < NR_OF_TYPES; i++)
		{
			for (GameObject* g : gameObjects->at(i))
			{
				_renderModule->RenderShadowMap(g->GetMatrix(), g->GetRenderObject());
			}
		}

		_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::LIGHT_APPLICATION);
		_renderModule->SetLightDataPerLight(_spotlights[i]);

		//Should transform the positions with world * camView * camProj. Use GeoVS? When shadowed, return (0,0,0,0)
		//_renderModule->DEBUG_RenderLightVolume(_spotlights[i]->GetVolumeBuffer(), _spotlights[i]->GetWorldMatrix(), _spotlights[i]->GetVertexCount(), _spotlights[i]->GetVertexSize());

		_renderModule->RenderScreenQuad();
	}

	/*-------------------------------------------------------- HUD and other 2D -----------------------------------------------------------*/

	//_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::HUD_PASS);

	//_renderModule->Render(_UI->GetTextureData());
	//_UI->Render(_renderModule->GetDeviceContext());

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