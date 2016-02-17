#include "GameLogic.h"
#include <DirectXMath.h>
#include "InputDevice.h"

GameLogic::GameLogic(ObjectHandler* objectHandler, System::Camera* camera, System::Controls* controls, PickingDevice* pickingDevice, GUI::UITree* uiTree, AssetManager* assetManager, System::SettingsReader* settingsReader)
{
	_objectHandler = objectHandler;
	_camera = camera;
	_controls = controls;
	_pickingDevice = pickingDevice;
	_settingsReader = settingsReader;

	_player = new Player(objectHandler);
	_objectHandler->InitPathfinding();
	_uiTree = uiTree;
	_assetManager = assetManager;
	_guardTexture = _assetManager->GetTexture("../Menues/PlacementStateGUI/units/Guardbutton1.png");
	_uiTree->GetNode("winscreen")->SetHidden(true);
	_uiTree->GetNode("losescreen")->SetHidden(true);
	_gameDone = false;
}

GameLogic::~GameLogic()
{
	delete _player;
	_player = nullptr;
}

void GameLogic::Update(float deltaTime)
{
	HandleInput(deltaTime);
	_objectHandler->Update(deltaTime);

	if (_objectHandler->GetRemainingToSpawn() <= 0 && !_gameDone)
	{
		if (_objectHandler->GetAllByType(LOOT).size() <= 0)				//You lost
		{
			_uiTree->GetNode("losescreen")->SetHidden(false);
			_gameDone = true;
		}
		else if (_objectHandler->GetAllByType(ENEMY).size() <= 0)		//You won
		{
			_uiTree->GetNode("winscreen")->SetHidden(false);
			_gameDone = true;
			_settingsReader->GetProfile()->_level += 1;
		}
	}
	_uiTree->GetNode("objectivetext")->SetText(L"Defeat the intruders! \n" + std::to_wstring(_objectHandler->GetAllByType(ENEMY).size()) + L" enemies still remain.");
}

bool GameLogic::IsGameDone() const
{
	return _gameDone;
}

/*
Private functions
*/
void GameLogic::HandleInput(float deltaTime)
{
	/*
	Select units
	*/
	HandleUnitSelect();

	/*
	Show selected units on the GUI
	*/
	ShowSelectedInfo();

	/*
	Handle patrol
	*/
	HandlePatrol();

	/*
	Move units
	*/
	HandleUnitMove();

	/*
	Toggle free camera mode
	*/
	HandleCamMode();

	/*
	Camera scroll
	*/
	HandleCamZoom(deltaTime);

	/*
	Camera rotation
	*/
	HandleCamRot();

	/*
	Camera move
	*/
	HandleCamMove(deltaTime);
}

void GameLogic::HandleUnitSelect()
{
	/*
	Select units
	*/
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		_pickingDevice->SetFirstBoxPoint(_controls->GetMouseCoord()._pos);
	}

	if (_controls->IsFunctionKeyUp("MOUSE:SELECT"))
	{
		//deselect everything first.
		vector<Unit*> units = _player->GetSelectedUnits();

		for (unsigned int i = 0; i < units.size(); i++)
		{
			units[i]->SetColorOffset(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
			if (units[i]->GetType() == GUARD)
			{
				for (auto p : ((Guard*)units[i])->GetPatrolRoute())
				{
					_objectHandler->GetTileMap()->GetObjectOnTile(p, FLOOR)->SetColorOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
				}
			}
		}
		_player->DeselectUnits();

		//Check if we picked anything
		vector<GameObject*> pickedUnits = _pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, _objectHandler->GetAllByType(GUARD));

		//if units selected
		if (pickedUnits.size() > 0)
		{
			//select
			for (unsigned int i = 0; i < pickedUnits.size(); i++)
			{
				_player->SelectUnit((Unit*)pickedUnits[i]);
				pickedUnits[i]->SetColorOffset(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
				if (pickedUnits[i]->GetType() == GUARD)
				{
					for (auto p : ((Guard*)pickedUnits[i])->GetPatrolRoute())
					{
						_objectHandler->GetTileMap()->GetObjectOnTile(p, FLOOR)->SetColorOffset(XMFLOAT3(0.0f, 0.0f, 1.0f));
					}
				}
			}
		}
	}
}

void GameLogic::HandlePatrol()
{
	//Set Guard Patrol Route if a Guard is Selected
	if (_controls->IsFunctionKeyDown("PLAY:SET_PATROL"))
	{
		if (_player->AreUnitsSelected())
		{
			AI::Vec2D tilePos = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);

			if (_objectHandler->GetTileMap()->IsFloorOnTile(tilePos))
			{
				_player->PatrolUnits(tilePos);
			}

			for (auto u : _player->GetSelectedUnits())
			{
				if (u->GetType() == GUARD)
				{
					for (auto p : ((Guard*)u)->GetPatrolRoute())
					{
						GameObject* patrolFloor = _objectHandler->GetTileMap()->GetObjectOnTile(p, FLOOR);
						if (patrolFloor != nullptr)
						{

							patrolFloor->SetColorOffset(XMFLOAT3(0.0f, 0.0f, 1.0f));
						}
					}
				}
			}
		}
	}
}

void GameLogic::HandleUnitMove()
{
	//Move units
	if (_controls->IsFunctionKeyDown("MOUSE:DESELECT"))
	{
		//Remove colour from patrolroute
		for (auto u : _player->GetSelectedUnits())
		{
			if (u->GetType() == GUARD)
			{
				for (auto p : ((Guard*)u)->GetPatrolRoute())
				{
					_objectHandler->GetTileMap()->GetObjectOnTile(p, FLOOR)->SetColorOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
				}
			}
		}


		int numUnits = _player->AreUnitsSelected();
		AI::Vec2D selectedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);
		//if more than 1 unit. Dont rotate and move them to tile.
		if (numUnits>1)
		{
			_player->MoveUnits(selectedTile);
		}
		//if one unit
		if (numUnits == 1)
		{
			//if tile is the same as he is on
			vector<Unit*> units = _player->GetSelectedUnits();
			if (selectedTile == units.at(0)->GetTilePosition())
			{
				//Check which direction he should be pointing

				AI::Vec2D direction = _pickingDevice->PickDirection(_controls->GetMouseCoord()._pos, _objectHandler->GetTileMap());

				//Change direction
				units.at(0)->SetDirection(direction);
			}
			//else move
			else
			{
				_player->MoveUnits(selectedTile);
			}
		}
	}
}

void GameLogic::ShowSelectedInfo()
{
	/*
	Show selected units on the GUI
	*/
	if (_player->AreUnitsSelected())
	{
		vector<Unit*> units = _player->GetSelectedUnits();
		int nrOfUnits = _player->GetNumberOfSelectedUnits();
		int healthSum = 0;

		_uiTree->GetNode("unitinfocontainer")->SetHidden(false);

		//TODO: Actually check if the units are guards //Mattias
		_uiTree->GetNode("unitinfotext")->SetText(L"Guard");
		_uiTree->GetNode("unitpicture")->SetTexture(_guardTexture);

		//Calculate health sum
		for (auto& i : units)
		{
			healthSum += i->GetHealth();
		}
		//TODO: Health is always 1? //Mattias
		_uiTree->GetNode("unithealth")->SetText(std::to_wstring(healthSum * 100) + L"%");

		//Show the number of units selected
		if (nrOfUnits > 1)
		{
			_uiTree->GetNode("unitnumber")->SetHidden(false);
			_uiTree->GetNode("unitnumber")->SetText(L"x " + std::to_wstring(nrOfUnits));
		}
		else
		{
			_uiTree->GetNode("unitnumber")->SetHidden(true);
		}
	}
	else
	{
		_uiTree->GetNode("unitinfocontainer")->SetHidden(true);
	}
}

void GameLogic::HandleCamMode()
{
	/*
	Toggle free camera mode
	*/
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
}

void GameLogic::HandleCamZoom(float deltaTime)
{
	/*
	Camera scroll
	*/
	const float ZOOM_MAX = 12.0f;
	const float ZOOM_MIN = 4.0f;
	float velocity = deltaTime / 20.0f;

	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		if (_controls->IsFunctionKeyDown("CAMERA:ZOOM_CAMERA_IN") &&
			(_camera->GetPosition().y - velocity) > ZOOM_MIN)
		{
			_camera->Move(_camera->GetForwardVector(), velocity);
		}
		else if (_controls->IsFunctionKeyDown("CAMERA:ZOOM_CAMERA_OUT") &&
			(_camera->GetPosition().y + velocity) < ZOOM_MAX)
		{
			XMFLOAT3 negForward = XMFLOAT3(
				_camera->GetForwardVector().x * -1,
				_camera->GetForwardVector().y * -1,
				_camera->GetForwardVector().z * -1);

			_camera->Move(negForward, velocity);
		}
	}
}

void GameLogic::HandleCamRot()
{
	/*
	Camera rotation
	*/
	bool isRotating = false;
	XMFLOAT3 rotation(0.0f, 0.0f, 0.0f);
	float rotV = 1.4f;

	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		if (_controls->IsFunctionKeyDown("CAMERA:ROTATE_CAMERA_LEFT"))
		{
			rotation.y = 1.0f;
			isRotating = true;
		}
		if (_controls->IsFunctionKeyDown("CAMERA:ROTATE_CAMERA_RIGHT"))
		{
			rotation.y = -1.0f;
			isRotating = true;
		}

		if (isRotating)
		{
			_camera->Rotate(XMFLOAT3((rotation.x * rotV), (rotation.y * rotV), (rotation.z * rotV)));
		}
	}

	//Camera rotation - locked mouse
	if (_controls->CursorLocked())
	{
		XMFLOAT3 rotation = _camera->GetRotation();
		rotation.x += _controls->GetMouseCoord()._deltaPos.y / 10.0f;
		rotation.y += _controls->GetMouseCoord()._deltaPos.x / 10.0f;

		_camera->SetRotation(rotation);
	}
}

void GameLogic::HandleCamMove(float deltaTime)
{
	/*
	Camera move
	*/
	XMFLOAT3 right(0.0f, 0.0f, 0.0f);
	XMFLOAT3 forward(0.0f, 0.0f, 0.0f);
	bool isMoving = false;
	float v = 0.06f + (_camera->GetPosition().y * 0.01);

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_UP"))
	{
		if (_camera->GetMode() == System::FREE_CAM)
		{
			forward = _camera->GetForwardVector();
		}
		else if (_camera->GetMode() == System::LOCKED_CAM)
		{
			forward = (Vec3(_camera->GetRightVector()).Cross(Vec3(XMFLOAT3(0.0f, 1.0f, 0.0f))).convertToXMFLOAT());
		}

		isMoving = true;
	}

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_DOWN"))
	{
		if (_camera->GetMode() == System::FREE_CAM)
		{
			forward = _camera->GetForwardVector();
		}
		else if (_camera->GetMode() == System::LOCKED_CAM)
		{
			forward = (Vec3(_camera->GetRightVector()).Cross(Vec3(XMFLOAT3(0.0f, 1.0f, 0.0f))).convertToXMFLOAT());
		}

		forward.x *= -1;
		forward.y *= -1;
		forward.z *= -1;
		isMoving = true;
	}

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_RIGHT"))
	{
		right = _camera->GetRightVector();
		isMoving = true;
	}

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_LEFT"))
	{
		right = _camera->GetRightVector();
		right.x *= -1;
		right.y *= -1;
		right.z *= -1;
		isMoving = true;
	}

	if (isMoving)
	{
		_camera->Move(XMFLOAT3((forward.x + right.x) * v, (forward.y + right.y) * v, (forward.z + right.z) * v), deltaTime / 10);
	}
}
