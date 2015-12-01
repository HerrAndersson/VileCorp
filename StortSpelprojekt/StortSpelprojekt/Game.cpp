#include "Game.h"
#include <stdexcept>
#include <DirectXMath.h>

Game::Game(HINSTANCE hInstance, int nCmdShow)
{
	_SM.Initialize();

	System::WindowSettings settings;
	_window = new System::Window("Amazing game", hInstance, settings);

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings._width, settings._height);

	_camera = new System::Camera(0.1f, 1000.0f, DirectX::XM_PIDIV2, settings._width, settings._height);
	_camera->SetPosition(XMFLOAT3(0, 0, -3));
	
	//_objectHandler = new ObjectHandler(_renderModule->GetDevice());
	
	_tempAssetManager = new AssetManager(_renderModule->GetDevice());
	
	_tempTestRenderObject = _tempAssetManager->GetRenderObject(1);
	stepValue = 0.0f;
}

Game::~Game() 
{	
	delete _window;
	delete _renderModule;
	delete _camera;
	//delete _objectHandler;
	delete _tempAssetManager;
}
void Game::ResizeResources(System::WindowSettings settings)
{
	_window->ResizeWindow(settings);
	_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
	_camera->Resize(settings._width, settings._height);
}

void Game::HandleInput()
{
	if (GetAsyncKeyState(VK_LEFT) != 0)
	{
		//Windowed mode
		System::WindowSettings settings(1280, 720, System::WindowSettings::SHOW_CURSOR);
		ResizeResources(settings);
	}

	if (GetAsyncKeyState(VK_RIGHT) != 0)
	{
		//Fullscreen
		System::WindowSettings settings(1920, 1080, System::WindowSettings::SHOW_CURSOR | System::WindowSettings::FULLSCREEN);
		ResizeResources(settings);
	}

	if (GetAsyncKeyState(VK_UP) != 0)
	{
		//Borderless windowed
		System::WindowSettings settings(567, 765, System::WindowSettings::SHOW_CURSOR | System::WindowSettings::BORDERLESS);
		ResizeResources(settings);
	}
}

void Game::Update()
{
	/*
	Object handler update

	h�mta fr�n objecthander eller olika update functioner i objecthander
	vi vill h�mta objekten

	*/

	
}

void Game::Render()
{
	stepValue += 0.001f;
	XMMATRIX i = XMMatrixRotationRollPitchYaw(stepValue, XM_PIDIV2, 0);
	_renderModule->BeginScene(0.0f, 1.0f, 1.0f, 1);
	_renderModule->SetResourcesPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());

	_renderModule->SetShaderStage(Renderer::RenderModule::GEO_PASS);
	_renderModule->Render(&i, _tempTestRenderObject);

	_renderModule->SetShaderStage(Renderer::RenderModule::LIGHT_PASS);
	_renderModule->RenderLightQuad();
	_renderModule->EndScene();
}

int Game::Run()
{
	while (_window->Run())
	{
		HandleInput();
		Update();
		Render();






	}

	return 0;
}