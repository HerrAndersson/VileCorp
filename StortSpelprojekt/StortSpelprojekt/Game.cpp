#include "Game.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

Game::Game(HINSTANCE hInstance, int nCmdShow)
{
	System::WindowSettings settings;
	_window = new System::Window("Amazing game", hInstance, settings);

	_timer = System::Timer();

	//Initialize Variables
	InitVar initVar;

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings._width, settings._height);

	_camera = new System::Camera(0.1f, 1000.0f, DirectX::XM_PIDIV2, settings._width, settings._height);
	_camera->SetPosition(XMFLOAT3(3, 10, 0));
	_camera->SetRotation(XMFLOAT3(60, 0, 0));
	initVar._camera = _camera;

	_UI = new UIHandler(_renderModule->GetDevice(), _window->GetWindowSettings());
	initVar._uiHandler = _UI;

	_objectHandler = new ObjectHandler(_renderModule->GetDevice());
	initVar._objectHandler = _objectHandler;
	
	_input = new System::InputDevice(_window->GetHWND());
	initVar._inputHandler = _input;

	_SM = new StateMachine(initVar);
}

Game::~Game() 
{	
	delete _window;
	delete _renderModule;
	delete _camera;
	delete _objectHandler;
	//TODO remove comments when the objectHandler is initialized
	//delete _objectHandler;
	delete _UI;
	delete _SM;
	delete _input;
}

void Game::ResizeResources(System::WindowSettings settings)
{
	_window->ResizeWindow(settings);
	_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
	_camera->Resize(settings._width, settings._height);
}

void Game::HandleInput()
{
	if (_input->IsPressed(System::Input::F10))
	{
		//Windowed mode
		System::WindowSettings settings(1280, 720, System::WindowSettings::SHOW_CURSOR);
		ResizeResources(settings);
	}

	if (_input->IsPressed(System::Input::F12))
	{
		//Fullscreen
		System::WindowSettings settings(1920, 1080, System::WindowSettings::SHOW_CURSOR | System::WindowSettings::FULLSCREEN);
		ResizeResources(settings);
	}

	if (_input->IsPressed(System::Input::F13))
	{
		//Borderless windowed
		System::WindowSettings settings(567, 765, System::WindowSettings::SHOW_CURSOR | System::WindowSettings::BORDERLESS);
		ResizeResources(settings);
	}
	if (_input->IsPressed(VK_ESCAPE))
	{
		//TODO: Make a graceful exit. Rikhard
	}
}

void Game::Update(float deltaTime)
{
	/*
	Object handler update

	hämta från objecthander eller olika update functioner i objecthander
	vi vill hämta objekten

	*/
	_input->Update();

	//Test code - Rikhard
	HandleInput();

	_UI->Update();
	_UI->OnResize(_window->GetWindowSettings());
	_SM->Update(deltaTime);
}

void Game::Render()
{
	_renderModule->BeginScene(0.0f, 1.0f, 1.0f, 1);
	_renderModule->SetResourcesPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());
	_renderModule->SetShaderStage(Renderer::RenderModule::GEO_PASS);
	
	std::vector<GameObject*>* gameObjects = _objectHandler->GetGameObjects();
	for (auto i : *gameObjects)
	{
		_renderModule->Render(&i->GetMatrix(), i->GetRenderObject());
	}

	_renderModule->SetShaderStage(Renderer::RenderModule::LIGHT_PASS);

	_renderModule->RenderLightQuad();
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