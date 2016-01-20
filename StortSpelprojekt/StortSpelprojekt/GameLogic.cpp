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

void GameLogic::Initialize(System::InputDevice* inputDevice, ObjectHandler* objectHandler, UIHandler* uiHandler, System::Camera* camera, System::Controls*	controls, PickingDevice* pickingDevice)
{
	_inputDevice = inputDevice;
	_objectHandler = objectHandler;
	_uiHandler = uiHandler;
	_camera = camera;
	_controls = controls;
	_pickingDevice = pickingDevice;

	//Needs a Tilemap in the objectHandler /Markus
	//_objectHandler->Add(TRAP, 0, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));

	//_objectHandler->Add(TRAP, 0, XMFLOAT3(0.5f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	_objectHandler->LoadLevel(6);

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

	//Picking control
	if (_inputDevice->IsPressed(System::Input::LeftMouse))
	{
		if (_player->AreUnitsSelected())
		{
			_player->MoveUnits(_pickingDevice->pickTile(_inputDevice->GetMouseCoord()._pos));
		}
		else
		{
			vector<GameObject*> pickedUnits = _pickingDevice->pickObjects(_inputDevice->GetMouseCoord()._pos, _objectHandler->GetAllByType(ENEMY));

			if (!pickedUnits.empty())
			{
				Unit* unit = (Unit*)pickedUnits[0];
				_player->SelectUnit(unit);

				unit->SetScale(DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f));
			}
		}
	}
	if (_inputDevice->IsPressed(System::Input::RightMouse))
	{
		if (_player->AreUnitsSelected())
		{
			vector<Unit*> units = _player->GetSelectedUnits();
			for (int i = 0; i < units.size(); i++)
			{
				units[i]->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			}
			_player->DeselectUnits();
		}
	}

	XMFLOAT3 forward(0, 0, 0);
	XMFLOAT3 position = _camera->GetPosition();
	XMFLOAT3 right(0, 0, 0);
	bool isMoving = false;
	float v = 0.1f;


	if (_controls->IsFunctionKeyDown("DEBUG:ENABLE_FREECAM"))
	{
		//_inputDevice->ToggleCursorLock();
	}

	if (_controls->IsFunctionKeyDown("DEBUG:MOVE_CAMERA_UP"))
	{
		forward = _camera->GetForwardVector();
		isMoving = true;
	}
	else if (_controls->IsFunctionKeyDown("DEBUG:MOVE_CAMERA_DOWN"))
	{
		forward = _camera->GetForwardVector();
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
}