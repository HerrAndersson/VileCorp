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
}

Game::~Game() 
{	
	delete _window;
	delete _renderModule;
	delete _camera;
}
void Game::ResizeResources(System::WindowSettings settings)
{
	_window->ResizeWindow(settings);
	_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
	_camera->Resize(settings._width, settings._height);
}


int Game::Run()
{
	while (_window->Run())
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


		_renderModule->SetResourcesPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());

		_renderModule->BeginScene(0.0f, 1.0f, 1.0f, 1);

		_renderModule->SetShaderStage(Renderer::RenderModule::GEO_PASS);
		//_renderModule->Render();

		_renderModule->SetShaderStage(Renderer::RenderModule::LIGHT_PASS);
		_renderModule->RenderLightQuad();
		_renderModule->EndScene();

	}

	return 0;
}