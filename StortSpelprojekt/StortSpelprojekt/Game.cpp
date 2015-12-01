#include "Game.h"
#include <stdexcept>

Game::Game(HINSTANCE hInstance, int nCmdShow)
{
	_SM.Initialize();

	System::WindowSettings settings;
	_window = new System::Window("Amazing game", hInstance, settings);

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings._width, settings._height);
}

Game::~Game() 
{	
	delete _window;
	delete _renderModule;
	delete _objectHandler;
}

void Game::HandleInput()
{
	if (GetAsyncKeyState(VK_LEFT) != 0)
	{
		//Windowed mode
		System::WindowSettings settings(1280, 720, System::WindowSettings::SHOW_CURSOR);
		_window->ResizeWindow(settings);
		_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
	}

	if (GetAsyncKeyState(VK_RIGHT) != 0)
	{
		//Fullscreen
		System::WindowSettings settings(1920, 1080, System::WindowSettings::SHOW_CURSOR | System::WindowSettings::FULLSCREEN);
		_window->ResizeWindow(settings);
		_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
	}

	if (GetAsyncKeyState(VK_UP) != 0)
	{
		//Borderless windowed
		System::WindowSettings settings(567, 765, System::WindowSettings::SHOW_CURSOR | System::WindowSettings::BORDERLESS);
		_window->ResizeWindow(settings);
		_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
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
	_renderModule->BeginScene(0.0f, 1.0f, 1.0f, 1);
		_renderModule->SetShaderStage(Renderer::RenderModule::GEO_PASS);
	_renderModule->Render();
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