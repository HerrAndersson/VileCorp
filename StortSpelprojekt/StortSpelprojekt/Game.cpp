#include"Game.h"

Game::Game() {}

Game::~Game() 
{
	delete _window;
	delete _renderModule;
}

HRESULT Game::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	HRESULT hr = S_OK;

	_SM.Initialize();

	System::WindowSettings settings;
	_window = new System::Window("A", hInstance, settings);

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings._width, settings._height);


	return hr;
}

int Game::Run()
{
	while (_window->Run())
	{
		if (GetAsyncKeyState(VK_LEFT) != 0)
		{
			System::WindowSettings settings(1280*4, 720*4, false, false, true);
			_window->ResizeWindow(settings);
			_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
		}

		if (GetAsyncKeyState(VK_RIGHT) != 0)
		{
			//Fullscreen
			System::WindowSettings settings(1920, 1080, true, false, true);
			_window->ResizeWindow(settings);
			_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
		}

		if (GetAsyncKeyState(VK_UP) != 0)
		{
			System::WindowSettings settings(567, 765, false, true, true);
			_window->ResizeWindow(settings);
			_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
		}


		_renderModule->BeginScene(1, 1, 0, 1);
		_renderModule->Render();
		_renderModule->EndScene();

	};

	return 0;
}