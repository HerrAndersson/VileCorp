#include "Game.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

Game::Game(HINSTANCE hInstance, int nCmdShow)
{
	System::WindowSettings settings;
	_window = new System::Window("Amazing game", hInstance, settings);

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings._width, settings._height);
	_UI = new UIHandler(_renderModule->GetDevice(), _window->GetWindowSettings());

	//Initialize Variables
	InitVar initVar;
	initVar._objectHandler	= _objectHandler;

	_camera = new System::Camera(0.1f, 1000.0f, DirectX::XM_PIDIV2, settings._width, settings._height);
	_camera->SetPosition(XMFLOAT3(3, 10, 0));
	_camera->SetRotation(XMFLOAT3(60, 0, 0));

	_timer = System::Timer();
	
	_objectHandler = new ObjectHandler(_renderModule->GetDevice());
	initVar._uiHandler = _UI;
	_SM = new StateMachine(initVar);

	_input = new System::InputDevice(_window->GetHWND());
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

	//Camera mouse control
	System::MouseCoord mouseCoord = _input->GetMouseCoord();
	if (mouseCoord._deltaPos.x != 0 || mouseCoord._deltaPos.y != 0)
	{
		XMFLOAT3 rotation = _camera->GetRotation();
		rotation.y += mouseCoord._deltaPos.x / 10.0f;
		rotation.x += mouseCoord._deltaPos.y / 10.0f;
		_camera->SetRotation(rotation);
	}

	XMFLOAT3 forward(0, 0, 0);
	XMFLOAT3 position = _camera->GetPosition();
	XMFLOAT3 right(0, 0, 0);
	bool isMoving = false;
	float v = 0.1f;
	if (GetAsyncKeyState('W'))
	{
		forward = _camera->GetForwardVector();
		isMoving = true;
	}
	else if (GetAsyncKeyState('S'))
	{
		forward = _camera->GetForwardVector();
		forward.x *= -1;
		forward.y *= -1;
		forward.z *= -1;
		isMoving = true;
	}

	if (GetAsyncKeyState('D'))
	{
		right = _camera->GetRightVector();
		isMoving = true;
	}
	else if (GetAsyncKeyState('A'))
	{
		right = _camera->GetRightVector();
		right.x *= -1;
		right.y *= -1;
		right.z *= -1;
		isMoving = true;
	}

	if (isMoving)
	{
		_camera->SetPosition(XMFLOAT3(position.x + (forward.x + right.x) * v, position.y + (forward.y + right.y) * v, position.z + (forward.z + right.z) * v));
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
	_UI->Update();
	_UI->OnResize(_window->GetWindowSettings());
	_SM->Update(deltaTime);
	_objectHandler->Update();
}

void Game::Render()
{
	_renderModule->BeginScene(0.0f, 1.0f, 1.0f, 1);
	_renderModule->SetResourcesPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());
	_renderModule->SetShaderStage(Renderer::RenderModule::GEO_PASS);



	//RenderList renderList = _objectHandler->GetAll(0);
	
	
	//_renderModule->Render(&_objectHandler->GetAll(TRAP).at(0)->GetMatrix(), renderList._renderObject);
	//_renderModule->Render(&_objectHandler->GetAll(TRAP).at(1)->GetMatrix(), renderList._renderObject);

	//TODO: Will make nestled later /Texxarne
	//for (int j = 0; j < renderList.modelMatrices.size(); j++)
	//{
	//	_renderModule->Render(&renderList.modelMatrices[j], renderList._renderObject);
	//}
	
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
	float deltaTime = 0; //someone create this.
	//Needs a Tilemap in the objectHandler /Markus
	//_objectHandler->Add(TRAP, 0, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	//_objectHandler->Add(TRAP, 0, XMFLOAT3(0.5f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	_objectHandler->LoadLevel(3);


	_objectHandler->InitPathfinding();

	while (_window->Run())
	{
		_timer.Update();
		if (_timer.GetFrameTime() >= MS_PER_FRAME)
		{
			HandleInput();
			Update(deltaTime);
			Render();
			_timer.Reset();
		}
	}

	return 0;
}