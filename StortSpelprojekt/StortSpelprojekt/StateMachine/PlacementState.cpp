#include "PlacementState.h"

void PlacementState::EvaluateGoldCost()
{
	int costOfAnvilTrap = 50;
	int costOfTeslaCoil = 100;
	int costOfCamera = 80;
	int costOfGuard = 200;

	if (_toPlace._blueprint->_subType == SPIKE)
	{
		_toPlace._goldCost = costOfAnvilTrap;
	}
	else if (_toPlace._blueprint->_subType == TESLACOIL)
	{
		_toPlace._goldCost = costOfTeslaCoil;
	}

	if (_toPlace._blueprint->_type == CAMERA)
	{
		_toPlace._goldCost = costOfCamera;
	}
	else if (_toPlace._blueprint->_type == GUARD)
	{
		_toPlace._goldCost = costOfGuard;
	}
}

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "PLACEMENT", assetManager, fontWrapper, settingsReader, soundModule)
{
	_playerProfile.resize(1);
	
	
	//load all player profiles
	GetFilenamesInDirectory("Assets/PlayerProfiles/", ".json", _playerProfilesPath);
	
	//rezise vector that stores player profiles
	_playerProfile.resize(_playerProfilesPath.size());
	_controls = controls;
	_objectHandler = objectHandler;
	_camera = camera;
	_pickingDevice = pickingDevice;
	_baseEdit = nullptr;
	
	_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));
}


PlacementState::~PlacementState()
{
	delete _baseEdit;
	_baseEdit = nullptr;
}

void PlacementState::Update(float deltaTime)
{
	_baseEdit->Update(deltaTime);

	HandleInput();
	HandleButtons();
}

void PlacementState::OnStateEnter()
{
	for (int i = 0; i < _playerProfilesPath.size(); i++)
	{
		System::loadJSON(&_playerProfile[i], _playerProfilesPath[i]);
	}
	//TODO: Temporary solution for budget problem when changing back to placement state (Previously it kept spent budget and did not reset it)
	_playerProfile[_currentPlayer]._gold = 700;
	_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));
	_baseEdit = new BaseEdit(_objectHandler, _controls, _pickingDevice, _camera, false);
	_objectHandler->DisableSpawnPoints();

	XMFLOAT3 campos;
	campos.x = _objectHandler->GetTileMap()->GetWidth() / 2;
	campos.y = 15;
	campos.z = _objectHandler->GetTileMap()->GetHeight() / 2 - 10;
	_camera->SetPosition(campos);

	//if (_playerProfile[0]._firstTime != true)
	//{
	_uiTree.GetNode("Tutorial")->SetHidden(false);
	//}
}

void PlacementState::OnStateExit()
{
	delete _baseEdit;
	_baseEdit = nullptr;
}

void PlacementState::HandleInput()
{
	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		ChangeState(PAUSESTATE);
	}

	/*
	if (_baseEdit->GetMarkedObject() != nullptr)
		{
		_toPlace._type = _baseEdit->GetMarkedObject()->GetType();
		_toPlace._subType =_baseEdit->GetMarkedObject()->GetSubType();
		_toPlace._blueprintID = _baseEdit->GetMarkedObject()->GetID();
			EvaluateGoldCost();
			if (_controls->IsFunctionKeyDown("MAP_EDIT:DELETE_UNIT"))
			{
				_playerProfile[_currentPlayer]._gold += _toPlace._goldCost;
				_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));
			_baseEdit->DeleteMarkedObject();
				_toPlace.ResetTemps();
			}
		}
	*/


	// placement invalid
	if (_toPlace._goldCost != -1 && !_objectHandler->Find(_toPlace._blueprint->_type, _toPlace._markerID) && !_baseEdit->IsPlace())
	{
		_playerProfile[_currentPlayer]._gold += _toPlace._goldCost;
		_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));
		_toPlace.ResetTemps();
	}
}

void PlacementState::HandleButtons()
{
	bool create = false;

	System::MouseCoord coord = _controls->GetMouseCoord();

	if (_uiTree.IsButtonColliding("Tutorial", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		//Hide how to screen when clicked
		_uiTree.GetNode("Tutorial")->SetHidden(true);
		_playerProfile[_currentPlayer]._firstTime = false;
		System::saveJSON(&_playerProfile[_currentPlayer], _playerProfilesPath[_currentPlayer], "Player Profile");
	}



	if (_uiTree.IsButtonColliding("Play", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		ChangeState(PLAYSTATE);
	}

	if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
	{

		if (_uiTree.IsButtonColliding("AnvilTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			_toPlace._blueprint = _objectHandler->GetBlueprintByType(TRAP, SPIKE);
			create = true;
		}
		if (_uiTree.IsButtonColliding("TeslaTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			_toPlace._blueprint = _objectHandler->GetBlueprintByType(TRAP, TESLACOIL);
			create = true;
		}
		if (_uiTree.IsButtonColliding("Camera", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			_toPlace._blueprint = _objectHandler->GetBlueprintByType(CAMERA, 0);
			create = true;
		}

		if (_uiTree.IsButtonColliding("Guard", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			_toPlace._blueprint = _objectHandler->GetBlueprintByType(GUARD, SHARK);
			create = true;
		}
	}

	if (create)
	{
		EvaluateGoldCost();

		if (_playerProfile[_currentPlayer]._gold >= _toPlace._goldCost)
		{
			_baseEdit->CreateMarker(_toPlace._blueprint, 666);
			_playerProfile[_currentPlayer]._gold -= _toPlace._goldCost;
			_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));
			_toPlace._markerID = _baseEdit->GetMarkedObject()->GetID();
		}
		else
		{
			_toPlace._goldCost = -1;
		}
	}
}


