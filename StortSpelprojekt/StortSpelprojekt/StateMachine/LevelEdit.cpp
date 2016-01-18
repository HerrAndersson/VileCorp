#include "LevelEdit.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>

LevelEdit::LevelEdit()
{}

LevelEdit::~LevelEdit()
{}

int LevelEdit::GetSelectedObject()
{
	return _selectedObj;
}

void LevelEdit::Add(Type type, int renderObjectID)
{
	_objectHandler->Add(type, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	
	GameObject* temp = _objectHandler->GetGameObjects()->at(type).back();
	_selectedObj = temp->GetID();
}

void LevelEdit::Initialize(InitVar* initVar)
{
	_inputHandler = initVar->_inputHandler;
	_objectHandler = initVar->_objectHandler;
	_uiHandler = initVar->_uiHandler;
	_camera = initVar->_camera;

	_selectedObj = 0;
	_lastSelected = 0;
	_tileMultiplier = 1;

	_objectHandler->LoadLevel(3);

	//_grid = new Grid(_renderModule->GetDevice(), 1, 10);
}

void LevelEdit::LoadLevel()
{
	//load existing level.
}

void LevelEdit::HandleInput()
{
	GameObject* temp;
	int maxObject = _objectHandler->GetObjectCount();
	int selectedLevel = 1;


	//Tile select
	if (_inputHandler->IsPressed(VK_ADD))
	{
		if (_selectedObj < maxObject)
		{
			_selectedObj++;
		}
		
	}

	if (_inputHandler->IsPressed(VK_SUBTRACT))
	{
		if (_selectedObj != 0)
		{
			_selectedObj--;
		}
			
	}

	//Move tile greater distance per button press
	if (_inputHandler->IsPressed(VK_PRIOR))
	{
		_tileMultiplier++;
	}

	if (_inputHandler->IsPressed(VK_NEXT))
	{
		if (_tileMultiplier != 1)
		{
			_tileMultiplier--;
		}
	}


	//Check if there is a tile to move
	if (_objectHandler->GetObjectCount() > 0)
	{
		//Translation and rotation controls
		if (_inputHandler->IsPressed(VK_LEFT))
		{
			
			temp = _objectHandler->Find(_selectedObj);
			if (temp->GetType() == WALL || FLOOR || UNIT)
			{
				XMFLOAT3 tempPos = temp->GetPosition();
				temp->SetPosition(XMFLOAT3(tempPos.x - 1 * _tileMultiplier, tempPos.y, tempPos.z));
			}

		}

		if (_inputHandler->IsPressed(VK_RIGHT))
		{
			temp = _objectHandler->Find(_selectedObj);
			if (temp->GetType() == WALL || FLOOR || UNIT)
			{
				XMFLOAT3 tempPos = temp->GetPosition();
				temp->SetPosition(XMFLOAT3(tempPos.x + 1 * _tileMultiplier, tempPos.y, tempPos.z));
			}
		}

		if (_inputHandler->IsPressed(VK_UP))
		{
			temp = _objectHandler->Find(_selectedObj);

			if (temp->GetType() == WALL || FLOOR || UNIT)
			{
				XMFLOAT3 tempPos = temp->GetPosition();
				temp->SetPosition(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z + 1 * _tileMultiplier));
			}
		}


		if (_inputHandler->IsPressed(VK_DOWN))
		{
			temp = _objectHandler->Find(_selectedObj);
			if (temp->GetType() == WALL || FLOOR || UNIT)
			{
				XMFLOAT3 tempPos = temp->GetPosition();
				temp->SetPosition(XMFLOAT3(tempPos.x, tempPos.y, tempPos.z - 1 * _tileMultiplier));
			}
		}
	}
	//Scale Objects
	if (_inputHandler->IsPressed(VK_NUMPAD6))
	{
		temp = _objectHandler->Find(_selectedObj);
		if (temp->GetType() == WALL || FLOOR || UNIT)
		{
			XMFLOAT3 tempPos = temp->GetScale();
			temp->SetScale(XMFLOAT3(tempPos.x+ 1, tempPos.y, tempPos.z ));
		}
	}

	if (_inputHandler->IsPressed(VK_NUMPAD4))
	{
		temp = _objectHandler->Find(_selectedObj);
		if (temp->GetType() == WALL || FLOOR || UNIT)
		{
			XMFLOAT3 tempPos = temp->GetScale();
			temp->SetScale(XMFLOAT3(tempPos.x - 1, tempPos.y, tempPos.z ));
		}
	}

	if (_inputHandler->IsPressed(VK_NUMPAD8))
	{
		temp = _objectHandler->Find(_selectedObj);
		if (temp->GetType() == WALL || FLOOR || UNIT)
		{
			XMFLOAT3 tempPos = temp->GetScale();
			temp->SetScale(XMFLOAT3(tempPos.x , tempPos.y, tempPos.z + 1));
		}
	}

	if (_inputHandler->IsPressed(VK_NUMPAD2))
	{
		temp = _objectHandler->Find(_selectedObj);
		if (temp->GetType() == WALL || FLOOR || UNIT)
		{
			XMFLOAT3 tempPos = temp->GetScale();
			temp->SetScale(XMFLOAT3(tempPos.x, tempPos.y , tempPos.z - 1));
		}
	}
	

	if (_inputHandler->IsPressed(0x5A))
	{
		temp = _objectHandler->Find(_selectedObj);
		if (temp->GetType() == WALL || FLOOR || UNIT)
		{
			XMFLOAT3 tempRot = temp->GetRotation();
			temp->SetRotation(XMFLOAT3(tempRot.x, tempRot.y + (DirectX::XM_PI/2), tempRot.z));
		}
	}
	if (_inputHandler->IsPressed(0x58))
	{
		temp = _objectHandler->Find(_selectedObj);
		if (temp->GetType() == WALL || FLOOR || UNIT)
		{
			XMFLOAT3 tempRot = temp->GetRotation();
			temp->SetRotation(XMFLOAT3(tempRot.x, tempRot.y - (DirectX::XM_PI / 2), tempRot.z));
		}
	}

	//Temp LevelLoads
	if (_inputHandler->IsPressed(0x31))
	{
		_objectHandler->Release();
		ResetSelectedObj();
		_objectHandler->LoadLevel(1);
		
	}

	if (_inputHandler->IsPressed(0x32))
	{
		_objectHandler->Release();
		ResetSelectedObj();
		_objectHandler->LoadLevel(2);
	}

	if (_inputHandler->IsPressed(0x33))
	{
		_objectHandler->Release();
		ResetSelectedObj();
		_objectHandler->LoadLevel(3);
		
	}

	if (_inputHandler->IsPressed(0x34))
	{
		_objectHandler->Release();
		ResetSelectedObj();
		_objectHandler->LoadLevel(4);
		
	}


	//Press C to init new level
	if (_inputHandler->IsPressed(0x43))
	{
		InitNewLevel();
	}


	////tempAddObj

	////R Adds Floor
	//if (_inputHandler->IsPressed(0x52))
	//{
	//		_objectHandler->Add(FLOOR, FLOOR, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));	
	//		_selectedObj = _objectHandler->GetSize()-1;
	//}

	////T adds Wall
	//if (_inputHandler->IsPressed(0x54))
	//{
	//		_objectHandler->Add(WALL, WALL, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	//		_selectedObj = _objectHandler->GetSize() - 1;
	//}

	////Y adds Unit
	//if (_inputHandler->IsPressed(0x59))
	//{
	//		_objectHandler->Add(UNIT, UNIT, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	//		_selectedObj = _objectHandler->GetSize() - 1;
	//}


	if (_inputHandler->IsPressed(VK_DELETE))
	{
		DeleteObject();
	}

	//Camera mouse control
	System::MouseCoord mouseCoord = _inputHandler->GetMouseCoord();
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

void LevelEdit::Update()
{
	HandleSelected();
	HandleInput();
	_objectHandler->Update();
}

void LevelEdit::HandleSelected()
{
	GameObject* temp;

	if (_objectHandler->GetObjectCount() != 0)
	{
		temp = _objectHandler->Find(_lastSelected);

		if (_selectedObj != _lastSelected || _selectedObj == 0)
		{
			if (temp!= nullptr && temp->GetType() == WALL || FLOOR || UNIT)
			{
				if (_lastSelected != -1)
				{

					XMFLOAT3 tempPos = temp->GetPosition();
					temp->SetPosition(XMFLOAT3(tempPos.x, 0.0, tempPos.z));
					//XMFLOAT3 tempScale = temp->GetScale();
					//temp->SetScale(XMFLOAT3(1, 1, 1));
				}
				temp = _objectHandler->Find(_selectedObj);
				XMFLOAT3 tempPos = temp->GetPosition();
				temp->SetPosition(XMFLOAT3(tempPos.x, 0.1, tempPos.z));
				//XMFLOAT3 tempScale = temp->GetScale();
				//temp->SetScale(XMFLOAT3(1.1, 1.1, 1.1));
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
	_objectHandler->Release();

	ResetSelectedObj();

	//Add(TRAP, TRAP);
}

void LevelEdit::DeleteObject()
{
	if (_selectedObj != 0)
	{
		_objectHandler->Remove(_selectedObj);
		_selectedObj--;
	}
}