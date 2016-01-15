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
	_camera->SetPosition(XMFLOAT3(3, 10, 0));
	_camera->SetRotation(XMFLOAT3(60, 0, 0));

	_UI = new UIHandler(_renderModule->GetDevice(), _window->GetWindowSettings(), _assetManager);
	
	_objectHandler = new ObjectHandler(_renderModule->GetDevice(), _assetManager);
	
	
	//Init statemachine
	_pickingDevice = new PickingDevice(_camera, _window);
	_SM = new StateMachine(_controls, _objectHandler, _UI, _input, _camera, _pickingDevice);	/*initVar._inputDevice = _input;*/

	_SM->Update(_timer.GetFrameTime());
	if (_SM->GetState() == LEVELEDITSTATE)
	{
		_grid = new Grid(_renderModule->GetDevice(), 1, 10);
	}
}

Game::~Game() 
{
	delete _window;
	delete _renderModule;
	delete _camera;
	delete _objectHandler;
	delete _UI;
	delete _SM;
	delete _controls;
	delete _assetManager;
	delete _pickingDevice;
	delete _input;

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
}



void Game::Update(float deltaTime)
{
	/*
	Object handler update

	hämta från objecthander eller olika update functioner i objecthander
	vi vill hämta objekten

	*/
	
	_UI->Update();
	_UI->OnResize(_window->GetWindowSettings());
	_SM->Update(deltaTime);

	_input->UpdatePerFrame();
}

void Game::Render()
{
	_renderModule->BeginScene(0.0f, 1.0f, 1.0f, 1);
	_renderModule->SetResourcesPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());
	_renderModule->SetShaderStage(Renderer::RenderModule::GEO_PASS);
	
	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (GameObject* g : gameObjects->at(i))
		{
			_renderModule->Render(&(g->GetMatrix()), g->GetRenderObject());
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

	_renderModule->SetShaderStage(Renderer::RenderModule::LIGHT_PASS);
	_renderModule->RenderLightQuad();
	
	_renderModule->SetShaderStage(Renderer::RenderModule::HUD_PASS);
	_renderModule->Render(_UI->GetTextureData());
	_UI->Render(_renderModule->GetDeviceContext());
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
		/*
		UINT dwSize;
		UINT asdf = 0;
		asdf = GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &dwSize,
			sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL)
		{
			return 0;
		}

		if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize,
			sizeof(RAWINPUTHEADER)) != dwSize)
			OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

		RAWINPUT* raw = (RAWINPUT*)lpb;
		delete[] lpb;
		return 0;
		*/
		_gameHandle->_input->Update(lparam, hwnd);
	}

	default:
	{
		return _gameHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}