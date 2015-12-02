#include "Game.h"
#include <stdexcept>

Game::Game(HINSTANCE hInstance, int nCmdShow)
{
	System::WindowSettings settings;
	_window = new System::Window("Amazing game", hInstance, settings);

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings._width, settings._height);
	_objectHandler = new ObjectHandler(_renderModule->GetDevice());
	_UI = new UIHandler(_renderModule->GetDevice());

	//Initialize Variables
	InitVar initVar;
	initVar.objectHandler	= _objectHandler;
	initVar.uiHandler		= _UI;
	_SM = new StateMachine(initVar);
}

Game::~Game() 
{	
	delete _window;
	delete _renderModule;
	delete _objectHandler;
	delete _UI;
	delete _SM;
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

void Game::Update(float deltaTime)
{
	/*
	Object handler update

	hämta från objecthander eller olika update functioner i objecthander
	vi vill hämta objekten

	*/

	_UI->Update();
	_SM->Update(deltaTime);
}

void Game::Render()
{
	_renderModule->BeginScene(0.0f, 1.0f, 1.0f, 1);
	_renderModule->SetShaderStage(0);
	_renderModule->Render();
	_renderModule->SetShaderStage(1);
	_renderModule->RenderLightQuad();
	_UI->Render(_renderModule->GetDeviceContext());
	_renderModule->EndScene();
}

int Game::Run()
{
	float deltaTime = 0; //someone create this.
	while (_window->Run())
	{
		HandleInput();
		Update(deltaTime);
		Render();
	}

	return 0;
}