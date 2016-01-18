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
	initVar.objectHandler	= _objectHandler;

	_camera = new System::Camera(0.1f, 1000.0f, DirectX::XM_PIDIV2, settings._width, settings._height);
	_camera->SetPosition(XMFLOAT3(3, 10, 0));
	_camera->SetRotation(XMFLOAT3(60, 0, 0));

	_timer = System::Timer();
	
	_objectHandler = new ObjectHandler(_renderModule->GetDevice());
	initVar.uiHandler		= _UI;
	_SM = new StateMachine(initVar);

	_input = new System::InputDevice(_window->GetHWND());

	//TEMP!
	_spotlight = new Renderer::Spotlight(_renderModule->GetDevice(), 0.1f, 1000.0f, XM_PIDIV4, 256, 256, 1.0f, 25.0f, XMFLOAT3(0.0f, 1.0f, 1.0f)); //Ska ha samma dimensions som shadow map, som nu ligger i render module
	_spotlight->SetPositionAndRotation(XMFLOAT3(15, 1.0f, 10), XMFLOAT3(0, 90, 0));
}

Game::~Game() 
{	
	delete _window;
	delete _renderModule;
	delete _camera;
	delete _objectHandler;
	delete _UI;
	delete _SM;
	delete _input;
	delete _spotlight;
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
	_input->Update();
	_UI->Update();
	_UI->OnResize(_window->GetWindowSettings());
	_SM->Update(deltaTime);
	_objectHandler->Update();
}

void Game::Render()
{
	_renderModule->BeginScene(0.0f, 1.0f, 1.0f, 1);

	///////////////////////////////////////////////////////////// Geometry pass //////////////////////////////////////////////////////////
	_renderModule->SetDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());
	_renderModule->SetShaderStage(Renderer::RenderModule::GEO_PASS);
	
	std::vector<GameObject*>* gameObjects = _objectHandler->GetGameObjects();

	for (auto i : *gameObjects)
	{
		_renderModule->Render(&i->GetMatrix(), i->GetRenderObject());
	}

	_renderModule->DEBUG_RenderLightVolume(_spotlight->GetVolumeBuffer());

	///////////////////////////////////////////////////////////// Light pass /////////////////////////////////////////////////////////////
	_renderModule->SetLightPassDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());
	//for (ALL SPOTLIGHTS)
	//{
		//Generates the shadow map for one spotlight
		//_renderModule->SetShaderStage(Renderer::RenderModule::SHADOW_GENERATION);
		//_renderModule->SetShadowMapDataPerSpotLight(_spotlight->GetViewMatrix(), _spotlight->GetProjectionMatrix());

		//for (auto i : *gameObjects)
		//{
		//	_renderModule->RenderShadowMap(&i->GetMatrix(), i->GetRenderObject());
		//}

		///*
		//Geo pass should render directly to the backbuffer.
		//Lighting pass should for each light:
		//	Create shadow map
		//	Render a volume, to backbuffer with additive blending, that represents the light to only do the calculations on the pixels that might be in the light. Apply shadow maps.

		//Then FINAL_PASS is not needed, just EndScene
		//*/

		//_renderModule->SetShaderStage(Renderer::RenderModule::LIGHT_APPLICATION);
		//_renderModule->SetLightPassDataPerLight(_spotlight);

	////}

	////////////////////////////////////////////////// Render quad to screen //////////////////////////////////////////////////////
	_renderModule->SetShaderStage(Renderer::RenderModule::FINAL_PASS);
	
	XMFLOAT3 rot = _spotlight->GetRotation();
	rot.y -= 2;
	_spotlight->SetRotation(rot);

	XMFLOAT3 color = _spotlight->GetColor();
	color.x = sin(_timer.GetGameTime() / 1000);
	color.y = sin(_timer.GetGameTime() / 1000 + XMConvertToRadians(120));
	color.z = sin(_timer.GetGameTime() / 1000 + XMConvertToRadians(240));
	_spotlight->SetColor(color);

	_renderModule->RenderScreenQuad();
	_renderModule->EndScene();
}

int Game::Run()
{
	_objectHandler->LoadLevel(5);
	_objectHandler->InitPathfinding();

	while (_window->Run())
	{
		//temp escape
		if (_input->IsPressed(VK_ESCAPE))
		{
			return 0;
		}

		_timer.Update();
		if (_timer.GetFrameTime() >= MS_PER_FRAME)
		{
			HandleInput();
			Update(_timer.GetFrameTime());

			Render();

			string s = to_string(_timer.GetFrameTime()) + " " + to_string(_timer.GetFPS());

			SetWindowText(_window->GetHWND(), s.c_str());

			_timer.Reset();
		}
	}

	return 0;
}