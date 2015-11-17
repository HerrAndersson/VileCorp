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

	return hr;
}

int Game::Run()
{
	return 1;
}