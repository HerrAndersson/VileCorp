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
		if (GetAsyncKeyState(VK_LEFT) != 0)
		{
			System::WindowSettings settings(1280, 720, false, false, true);
			_window->ResizeWindow(settings);
		}

		if (GetAsyncKeyState(VK_RIGHT) != 0)
		{
			System::WindowSettings settings(1280, 720, true, false, true);
			_window->ResizeWindow(settings);
		}

		if (GetAsyncKeyState(VK_UP) != 0)
		{
			System::WindowSettings settings(1280, 720, false, true, true);
			_window->ResizeWindow(settings);
		}
	};

	return 1;
}