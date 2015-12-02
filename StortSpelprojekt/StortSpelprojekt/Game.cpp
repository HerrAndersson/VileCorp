#include "Game.h"
#include <stdexcept>
#include <DirectXMath.h>

Game::Game(HINSTANCE hInstance, int nCmdShow)
{
	_SM.Initialize();

	System::WindowSettings settings;
	_window = new System::Window("Amazing game", hInstance, settings);

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings._width, settings._height);

	_camera = new System::Camera(0.1f, 1000.0f, DirectX::XM_PIDIV2, settings._width, settings._height);
	_camera->SetPosition(XMFLOAT3(0, 0, -15));
	_camera->Update();
	
	_objectHandler = new ObjectHandler(_renderModule->GetDevice());
	_timer = System::Timer();

	_inputDevice = System::InputDevice();

}

Game::~Game() 
{	
	delete _window;
	delete _renderModule;
	delete _camera;
	delete _objectHandler;
}
void Game::ResizeResources(System::WindowSettings settings)
{
	_window->ResizeWindow(settings);
	_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
	_camera->Resize(settings._width, settings._height);
}

void Game::HandleInput()
{
	_inputDevice.Update();
	if (_inputDevice.IsPressed(System::Input::LeftArrow))
	{
		//Windowed mode
		System::WindowSettings settings(1280, 720, System::WindowSettings::SHOW_CURSOR);
		ResizeResources(settings);
	}

	if (_inputDevice.IsPressed(System::Input::RightArrow))
	{
		//Fullscreen
		System::WindowSettings settings(1920, 1080, System::WindowSettings::SHOW_CURSOR | System::WindowSettings::FULLSCREEN);
		ResizeResources(settings);
	}

	if (_inputDevice.IsPressed(System::Input::UpArrow))
	{
		//Borderless windowed
		System::WindowSettings settings(567, 765, System::WindowSettings::SHOW_CURSOR | System::WindowSettings::BORDERLESS);
		ResizeResources(settings);
	}

	// Move the camera with keyboard (WIP)
	XMVECTOR up = XMLoadFloat3(&_camera->GetRotation()) * XMLoadFloat3(&XMFLOAT3(0.0, 1.0, 0.0));
	XMVECTOR forward = XMLoadFloat3(&_camera->GetRotation()) * XMLoadFloat3(&XMFLOAT3(0.0, 1.0, 0.0));
	XMVECTOR strafe = DirectX::XMVector3Cross(up, forward);

	if (_inputDevice.IsDown(System::Input::W))
	{
		XMVECTOR temp = XMLoadFloat3(&_camera->GetPosition()) * XMLoadFloat3(&XMFLOAT3(0.1, 0, -0.1));
		XMFLOAT3 temp2;
		DirectX::XMStoreFloat3(&temp2, temp);
		_camera->SetPosition(temp2);
		_camera->Update();
	}
	if (_inputDevice.IsDown(System::Input::A))
	{

	}
	if (_inputDevice.IsDown(System::Input::S))
	{
		XMVECTOR temp = XMLoadFloat3(&_camera->GetPosition()) * XMLoadFloat3(&XMFLOAT3(-0.1, 0, 0.1));
		XMFLOAT3 temp2;
		DirectX::XMStoreFloat3(&temp2, temp);
		_camera->SetPosition(temp2);
		_camera->Update();
	}
	if (_inputDevice.IsDown(System::Input::D))
	{

	}

	//void MoveCamera(Camera* cam, GLFWwindow* window)
	//{
	//	float cameraSpeed = 5;

	//	// Moved the camera with keyboard (WIP)
	//	glm::vec3 up = mat3(cam->GetRotationMatrix()) * glm::vec3(0.0, 1.0, 0.0);
	//	glm::vec3 forward = mat3(cam->GetRotationMatrix()) * glm::vec3(0.0, 0.0, -1.0);
	//	glm::vec3 strafe = glm::cross(up, forward);

	//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	//	{
	//		cam->UpdatePosition(forward*glm::vec3(0.1f, 0, -0.1)*vec3(cameraSpeed));
	//	}
	//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	//	{
	//		cam->UpdatePosition(forward*glm::vec3(-0.1f, 0, 0.1)*vec3(cameraSpeed));
	//	}
	//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	//	{
	//		cam->UpdatePosition(strafe*glm::vec3(0.1f, 0, -0.1f)*vec3(cameraSpeed));
	//	}
	//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	//	{
	//		cam->UpdatePosition(strafe*glm::vec3(-0.1f, 0, 0.1)*vec3(cameraSpeed));
	//	}
	//	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	//	{
	//		EndOfLine.AddPoint(-cam->GetPosition());
	//	}
	//}
}

void Game::Update()
{
	/*
	Object handler update

	hämta från objecthander eller olika update functioner i objecthander
	vi vill hämta objekten

	*/

	
}

void Game::Render()
{
	_renderModule->BeginScene(0.0f, 1.0f, 1.0f, 1);
	_renderModule->SetResourcesPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());

	_renderModule->SetShaderStage(Renderer::RenderModule::GEO_PASS);
	//_renderModule->Render(&i, _tempTestRenderObject);

	_renderModule->SetShaderStage(Renderer::RenderModule::LIGHT_PASS);
	_renderModule->RenderLightQuad();
	_renderModule->EndScene();
}

int Game::Run()
{
	while (_window->Run())
	{
		_timer.Update();
		if (_timer.GetFrameTime() >= MS_PER_FRAME)
		{
			HandleInput();
			Update();
			Render();
			_timer.Reset();
		}
	}

	return 0;
}