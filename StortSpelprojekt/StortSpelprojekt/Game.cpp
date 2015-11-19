#include"Game.h"

Game::Game() {}

Game::~Game() 
{
	Release();

	delete _window;
}

void Game::Release() {}

HRESULT Game::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	HRESULT hr = S_OK;

	_SM.Initialize();

	System::WindowSettings settings;
	_window = new System::Window("A", hInstance, settings);

	return hr;
}

int Game::Run()
{
	int i = 1;
	while (_window->Run()) 
	{ 
		if (i == 1)
		{
			System::WindowSettings settings(1280, 720, false, false, true);
			_window->ResizeWindow(settings);
			//Resize rendertarget etc here
			//Ex. Renderer::ResizeBuffers();
			i = 0;
		}
	};

	return 1;
}