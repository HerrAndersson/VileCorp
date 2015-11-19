#include"Game.h"

Game::Game() {}

Game::~Game() 
{
	Release();
}

void Game::Release() {}

HRESULT Game::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	HRESULT hr = S_OK;

	_SM.Initialize();
	_window = System::Window("A", hInstance);

	return hr;
}

int Game::Run()
{
	while (true) 
	{
		_window.Run();
	};
	return 0;
}