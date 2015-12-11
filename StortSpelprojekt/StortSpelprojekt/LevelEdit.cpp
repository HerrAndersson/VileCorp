#include "LevelEdit.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

LevelEdit::LevelEdit(HINSTANCE hInstance, int nCmdShow)
{
	System::WindowSettings settings;
	_window = new System::Window("Amazing LevelEditor", hInstance, settings);

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings._width, settings._height);
	_UI = new UIHandler(_renderModule->GetDevice(), _window->GetWindowSettings());

	//Initialize Variables
	InitVar initVar;
	initVar.objectHandler = _objectHandler;

	_camera = new System::Camera(0.1f, 1000.0f, DirectX::XM_PIDIV2, settings._width, settings._height);
	_camera->SetPosition(XMFLOAT3(3, 10, 0));
	_camera->SetRotation(XMFLOAT3(60, 0, 0));

	_timer = System::Timer();

	_objectHandler = new ObjectHandler(_renderModule->GetDevice());
	initVar.uiHandler = _UI;
	_SM = new StateMachine(initVar);

	_input = new System::InputDevice(_window->GetHWND());

	_selectedObj = 0;
	_lastSelected = 0;
	_tileMultiplier = 1;

	
}

LevelEdit::~LevelEdit()
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
void LevelEdit::ResizeResources(System::WindowSettings settings)
{
	_window->ResizeWindow(settings);
	_renderModule->ResizeResources(_window->GetHWND(), settings._width, settings._height);
	_camera->Resize(settings._width, settings._height);
}


void LevelEdit::LoadLevel()
{
	//load existing level.
}


void LevelEdit::HandleInput()
{
	GameObject* temp;
	int maxObject = _objectHandler->GetSize();
	int selectedLevel = 1;


	//Tile select
	if (_input->IsPressed(VK_ADD))
	{
		if (_selectedObj < maxObject)
		{
			_selectedObj++;
		}
		
	}

	if (_input->IsPressed(VK_SUBTRACT))
	{
		if (_selectedObj != 0)
		{
			_selectedObj--;
		}
			
	}

	//Move tile greater distance per button press
	if (_input->IsPressed(VK_PRIOR))
	{
		_tileMultiplier++;
	}

	if (_input->IsPressed(VK_NEXT))
	{
		if (_tileMultiplier != 1)
		{
			_tileMultiplier--;
		}
	}

	//Translation and rotation controls
	if (_input->IsPressed(VK_LEFT))
	{

		temp = _objectHandler->Find(_selectedObj);
		if (temp->GetType() < 3)
		{
			XMFLOAT3 tempPos = temp->GetPosition();
			temp->SetPosition(XMFLOAT3(tempPos.x - 1 * _tileMultiplier, tempPos.y, tempPos.z));
		}

	}

	if (_input->IsPressed(VK_RIGHT))
	{
		temp = _objectHandler->Find(_selectedObj);
		if (temp->GetType()  < 3)
		{
			XMFLOAT3 tempPos = temp->GetPosition();
			temp->SetPosition(XMFLOAT3(tempPos.x + 1 * _tileMultiplier, tempPos.y, tempPos.z));
		}
	}

	if (_input->IsPressed(VK_UP))
	{
		temp = _objectHandler->Find(_selectedObj);

		if (temp->GetType()  < 3)
		{
			XMFLOAT3 tempPos = temp->GetPosition();
			temp->SetPosition(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z + 1 * _tileMultiplier));
		}
	}


	if (_input->IsPressed(VK_DOWN))
	{
		temp = _objectHandler->Find(_selectedObj);
		if (temp->GetType()  < 3)
		{
			XMFLOAT3 tempPos = temp->GetPosition();
			temp->SetPosition(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z - 1 * _tileMultiplier));
		}
	}


	if (_input->IsPressed(0x5A))
	{
		temp = _objectHandler->Find(_selectedObj);
		if (temp->GetType()  < 3)
		{
			XMFLOAT3 tempRot = temp->GetRotation();
			temp->SetRotation(XMFLOAT3(tempRot.x, tempRot.y + (DirectX::XM_PI/2), tempRot.z));
		}
	}
	if (_input->IsPressed(0x58))
	{
		temp = _objectHandler->Find(_selectedObj);
		if (temp->GetType()  < 3)
		{
			XMFLOAT3 tempRot = temp->GetRotation();
			temp->SetRotation(XMFLOAT3(tempRot.x, tempRot.y - (DirectX::XM_PI / 2), tempRot.z));
		}
	}

	//Temp LevelLoads
	if (_input->IsPressed(0x31))
	{
		_objectHandler->Clear();
		ResetSelectedObj();
		_objectHandler->LoadLevel(1);
		
	}

	if (_input->IsPressed(0x32))
	{
		_objectHandler->Clear();
		ResetSelectedObj();
		_objectHandler->LoadLevel(2);
	}

	if (_input->IsPressed(0x33))
	{
		_objectHandler->Clear();
		ResetSelectedObj();
		_objectHandler->LoadLevel(3);
		
	}

	if (_input->IsPressed(0x34))
	{
		_objectHandler->Clear();
		ResetSelectedObj();
		_objectHandler->LoadLevel(4);
		
	}


	//Press C to init new level
	if (_input->IsPressed(0x43))
	{
		InitNewLevel();
	}


	//tempAddObj

	//R Adds Floor
	if (_input->IsPressed(0x52))
	{
		if (_objectHandler->GetSize() < 1)
		{
			_objectHandler->Add(FLOOR, FLOOR, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));	
			_selectedObj = _objectHandler->GetSize()-1;
		}
		else
		{
			_objectHandler->Add(FLOOR, FLOOR, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			_selectedObj = _objectHandler->GetSize() - 1;
		}	
	}

	//T adds Wall
	if (_input->IsPressed(0x54))
	{
		if (_objectHandler->GetSize() < 1)
		{
			_objectHandler->Add(WALL, WALL, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			_selectedObj = _objectHandler->GetSize() - 1;

		}
		else
		{
			_objectHandler->Add(WALL, WALL, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			_selectedObj = _objectHandler->GetSize() - 1;
		}
	}

	//Y adds Unit
	if (_input->IsPressed(0x59))
	{
		if (_objectHandler->GetSize() < 1)
		{
			_objectHandler->Add(UNIT, UNIT, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			_selectedObj = _objectHandler->GetSize() - 1;
		}
		else
		{
			_objectHandler->Add(UNIT, UNIT, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			_selectedObj = _objectHandler->GetSize() - 1;
		}
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

void LevelEdit::Update(float deltaTime)
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

void LevelEdit::Render()
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

int LevelEdit::Run()
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

		if (_input->IsPressed(VK_ESCAPE))
		{
			return 0;

		}
		if (_timer.GetFrameTime() >= MS_PER_FRAME)
		{
			HandleInput();
			HandleSelected();
			Update(deltaTime);
			Render();
			_timer.Reset();
		}
	}

	return 0;
}


void LevelEdit::HandleSelected()
{
	GameObject* temp;

	if (_objectHandler->GetSize() != 0)
	{
		temp = _objectHandler->Find(_lastSelected);

		if (_selectedObj != _lastSelected || _selectedObj == 0)
		{
			if (_lastSelected != -1)
			{
				//Lower last selected
				temp = _objectHandler->Find(_lastSelected);
				if (temp->GetType() < 3)
				{
					XMFLOAT3 tempPos = temp->GetPosition();
					temp->SetPosition(XMFLOAT3(tempPos.x, 0.0, tempPos.z));
					XMFLOAT3 tempScale = temp->GetScale();
					temp->SetScale(XMFLOAT3(1, 1, 1));
				}
			}
			//Raise selected
			temp = _objectHandler->Find(_selectedObj);
			if (temp->GetType() < 3)
			{
				XMFLOAT3 tempPos = temp->GetPosition();
				temp->SetPosition(XMFLOAT3(tempPos.x, 0.2, tempPos.z));
				XMFLOAT3 tempScale = temp->GetScale();
				temp->SetScale(XMFLOAT3(1.2, 1.2, 1.2));
			}


		}
		

		_lastSelected = _selectedObj;
	}
}


void LevelEdit::ResetSelectedObj()
{
	_selectedObj = 0;
	_lastSelected = -1;
}

void LevelEdit::InitNewLevel()
{
	_objectHandler->Clear();
	//ResetSelectedObj();
}