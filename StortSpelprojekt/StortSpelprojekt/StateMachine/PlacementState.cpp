#include "PlacementState.h"

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
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
	_ambientLight = ambientLight;

	std::vector<GUI::Node*>* units = _uiTree.GetNode("Units")->GetChildren();
	for (int i = 0; i < units->size(); i++)
	{
		GUI::BlueprintNode* newUnit = new GUI::BlueprintNode(*units->at(i), _objectHandler->GetBlueprintByName(units->at(i)->GetId()), 0);
		delete units->at(i);
		units->at(i) = (GUI::Node*)newUnit;
	}
}

void PlacementState::EvaluateGoldCost()
{
	int costOfAnvilTrap = 50;
	int costOfTeslaCoil = 100;
	int costOfCamera = 80;
	int costOfGuard = 200;

	if (_toPlace._sB._blueprint->_subType == SPIKE)
	{
		_toPlace._goldCost = costOfAnvilTrap;
	}
	else if (_toPlace._sB._blueprint->_subType == TESLACOIL)
	{
		_toPlace._goldCost = costOfTeslaCoil;
	}
	if (_toPlace._sB._blueprint->_type == CAMERA)
	{
		_toPlace._goldCost = costOfCamera;
	}
	else if (_toPlace._sB._blueprint->_type == GUARD)
	{
		_toPlace._goldCost = costOfGuard;
	}
}

PlacementState::~PlacementState()
{
	delete _baseEdit;
	_baseEdit = nullptr;
}

void PlacementState::Update(float deltaTime)
{
	_baseEdit->Update(deltaTime, false);
	HandleCam(deltaTime);
	HandleInput();
	HandleButtons();
}

void PlacementState::OnStateEnter()
{
	_ambientLight->x = AMBIENT_LIGHT_DAY.x;
	_ambientLight->y = AMBIENT_LIGHT_DAY.y;
	_ambientLight->z = AMBIENT_LIGHT_DAY.z;

	for (int i = 0; i < _playerProfilesPath.size(); i++)
	{
		System::loadJSON(&_playerProfile[i], _playerProfilesPath[i]);
	}
	_budget = _objectHandler->GetCurrentLevelHeader()->_budget;
	
	//Fix so that budgetvalue won't get read if we go into pause state! We don't want the players to cheat themselves back to their budget money by pressing pause, resume, pause etc.. Enbom
	_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_budget));
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
	if (_baseEdit->DroppedObject())
	{
		if (_budget >= _toPlace._goldCost && _baseEdit->CreatedObject() != nullptr)
		{
			_budget -= _toPlace._goldCost;
			_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_budget));
		}
		else
		{
			_objectHandler->Remove(_baseEdit->CreatedObject());
		}
	}

	if (_baseEdit->DeletedObjectBlueprint() != nullptr)
	{
		_toPlace._sB._blueprint = _baseEdit->DeletedObjectBlueprint();
		EvaluateGoldCost();
		_budget += _toPlace._goldCost;
		_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_budget));
	}


	// placement invalid
	//if (_toPlace._goldCost != -1 && !_objectHandler->Find(_toPlace._sB._blueprint->_type, _toPlace._markerID) && !_baseEdit->IsPlace())
	//{
	//	_budget += _toPlace._goldCost;
	//	_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_budget));
	//	_toPlace.ResetTemps();
	//}
}

void PlacementState::HandleButtons()
{
	bool create = false;

	System::MouseCoord coord = _controls->GetMouseCoord();

	if (_uiTree.IsButtonColliding("Tutorial", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		//Hide how to screen when clicked
		_uiTree.GetNode("Tutorial")->SetHidden(true);
	}

	if (_uiTree.IsButtonColliding("Play", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		ChangeState(PLAYSTATE);
	}

	if (_baseEdit->IsSelection() && !_baseEdit->IsPlace() && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		std::vector<GUI::Node*>* units = _uiTree.GetNode("Units")->GetChildren();
		for (unsigned y = 0; y < units->size(); y++)
		{
			GUI::Node* currentButton = units->at(y);
			if (_uiTree.IsButtonColliding(currentButton, coord._pos.x, coord._pos.y))
			{
				GUI::BlueprintNode* currentBlueprintButton = static_cast<GUI::BlueprintNode*>(currentButton);
				_toPlace._sB._blueprint = currentBlueprintButton->GetBlueprint();
				_toPlace._sB._textureId = currentBlueprintButton->GetTextureId();
				create = true;
				break;
			}
		}
	}

	if (create)
	{
		EvaluateGoldCost();
		_baseEdit->HandleBlueprint(&_toPlace._sB);
		_toPlace._markerID = _baseEdit->GetMarkedObject()->GetID();

		//if (_budget >= _toPlace._goldCost)
		//{
		//	_baseEdit->HandleBlueprint(&_toPlace._sB);
		//	_budget -= _toPlace._goldCost;
		//	_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_budget));
		//	_toPlace._markerID = _baseEdit->GetMarkedObject()->GetID();
		//}
		//else
		//{
		//	_toPlace._goldCost = -1;
		//}
	}
}


