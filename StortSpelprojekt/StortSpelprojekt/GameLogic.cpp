#include "GameLogic.h"
#include <DirectXMath.h>


GameLogic::GameLogic()
{
	_player = nullptr;
}

GameLogic::~GameLogic()
{
	delete _player;
}

void GameLogic::Initialize(ObjectHandler* objectHandler, System::Camera* camera, System::Controls*	controls, PickingDevice* pickingDevice)
{
	_objectHandler = objectHandler;
	_camera = camera;
	_controls = controls;
	_pickingDevice = pickingDevice;

	//Needs a Tilemap in the objectHandler /Markus
	//_objectHandler->Add(TRAP, 0, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	//_objectHandler->Add(TRAP, 0, XMFLOAT3(0.5f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	_objectHandler->LoadLevel(1);

	_objectHandler->InitPathfinding();

	_player = new Player();
}

void GameLogic::Update(float deltaTime)
{
	HandleInput();
	_objectHandler->Update(deltaTime);
}

void GameLogic::HandleInput()
{
	//Selecting a Unit
	if (_controls->IsFunctionKeyDown("PLAY:SELECT"))
	{
		vector<GameObject*> pickedUnits = _pickingDevice->pickObjects(_controls->GetMouseCoord()._pos, _objectHandler->GetAllByType(GUARD));


		if (pickedUnits.empty())
		{
			if (_player->AreUnitsSelected())
			{
				_player->MoveUnits(_pickingDevice->pickTile(_controls->GetMouseCoord()._pos));
			}
		}
		else
		{
			vector<Unit*> units = _player->GetSelectedUnits();
			for (int i = 0; i < units.size(); i++)
			{
				units[i]->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			}
			_player->DeselectUnits();


			Unit* unit = (Unit*)pickedUnits[0];
			_player->SelectUnit(unit);

			unit->SetScale(DirectX::XMFLOAT3(1.2f, 1.2f, 1.2f));
		}
	}
	//Deselect units
	if (_controls->IsFunctionKeyDown("PLAY:DESELECT"))
	{
		if (_player->AreUnitsSelected())
		{
			vector<Unit*> units = _player->GetSelectedUnits();
			for (uint i = 0; i < units.size(); i++)
			{
				units[i]->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			}
			_player->DeselectUnits();
		}
	}

	//Boxselecting Units
	if(_controls->IsFunctionKeyDown("PLAY:BOX_SELECT"))
	{
		_pickingDevice->setFirstBoxPoint(_controls->GetMouseCoord()._pos);
	}

	if (_controls->IsFunctionKeyUp("PLAY:BOX_SELECT"))
	{
		vector<GameObject*> pickedUnits = _pickingDevice->boxPickObjects(_controls->GetMouseCoord()._pos, _objectHandler->GetAllByType(GUARD));

		for (int i = 0; i < pickedUnits.size(); i++)
		{
			_player->SelectUnit((Unit*)pickedUnits[i]);
			pickedUnits[i]->SetScale(DirectX::XMFLOAT3(1.2f, 1.2f, 1.2f));
		}

	}


	
	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		if (_controls->IsFunctionKeyDown("PLAY:SCROLLDOWN") &&
			_camera->GetPosition().y > 4.0f)
		{
			_camera->Move(XMFLOAT3(0.0f, -1.0f, 0.0f));
		}
		else if (_controls->IsFunctionKeyDown("PLAY:SCROLLUP") &&
			_camera->GetPosition().y < 12.0f)
		{
			_camera->Move(XMFLOAT3(0.0f, 1.0f, 0.0f));
		}
	}
	

	XMFLOAT3 forward(0, 0, 0);
	XMFLOAT3 position = _camera->GetPosition();
	XMFLOAT3 right(0, 0, 0);
	bool isMoving = false;
	float v = 0.06f + (_camera->GetPosition().y * 0.01);

	if (_controls->IsFunctionKeyDown("DEBUG:ENABLE_FREECAM"))
	{
		if (_camera->GetMode() == System::LOCKED_CAM)
		{
			_controls->ToggleCursorLock();
			_camera->SetMode(System::FREE_CAM);
		}
		else
		{
			_controls->ToggleCursorLock();
			_camera->SetMode(System::LOCKED_CAM);
			_camera->SetRotation(DirectX::XMFLOAT3(70, 0, 0));
		}
	}

	if (_controls->IsFunctionKeyDown("DEBUG:MOVE_CAMERA_UP"))
	{
		if (_camera->GetMode() == System::FREE_CAM)
		{
			forward = _camera->GetForwardVector();
		}
		else if (_camera->GetMode() == System::LOCKED_CAM)
		{
			forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
		}

		isMoving = true;
	}
	else if (_controls->IsFunctionKeyDown("DEBUG:MOVE_CAMERA_DOWN"))
	{
		if (_camera->GetMode() == System::FREE_CAM)
		{
			forward = _camera->GetForwardVector();
		}
		else if (_camera->GetMode() == System::LOCKED_CAM)
		{
			forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
		}

		forward.x *= -1;
		forward.y *= -1;
		forward.z *= -1;
		isMoving = true;
	}

	if (_controls->IsFunctionKeyDown("DEBUG:MOVE_CAMERA_RIGHT"))
	{
		right = _camera->GetRightVector();
		isMoving = true;
	}
	else if (_controls->IsFunctionKeyDown("DEBUG:MOVE_CAMERA_LEFT"))
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
	
	if (_controls->CursorLocked())
	{
		XMFLOAT3 rotation = _camera->GetRotation();
		rotation.x += _controls->GetMouseCoord()._deltaPos.y / 10.0f;
		rotation.y += _controls->GetMouseCoord()._deltaPos.x / 10.0f;

		_camera->SetRotation(rotation);
	}
	
}