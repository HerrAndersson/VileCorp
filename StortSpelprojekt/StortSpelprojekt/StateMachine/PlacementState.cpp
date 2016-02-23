#include "PlacementState.h"

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_playerProfile.resize(1);
	_tutorialLogic = nullptr;
	_baseEdit = nullptr;

	//Money
	_costOfAnvilTrap	= 50;
	_costOfTeslaCoil	= 100;
	_costOfCamera		= 80;
	_costOfGuard		= 200;

	//load all player profiles
	GetFilenamesInDirectory("Assets/PlayerProfiles/", ".json", _playerProfilesPath);

	//rezise vector that stores player profiles
	_playerProfile.resize(_playerProfilesPath.size());
	_controls = controls;
	_objectHandler = objectHandler;
	_camera = camera;
	_pickingDevice = pickingDevice;

	_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));

	//if tutorial should initialize
	_tutorialLogic = new TutorialLogic(&_uiTree, _controls);
}

void PlacementState::EvaluateGoldCost()
{
	if (_toPlace._subType == SPIKE)
	{
		_toPlace._goldCost = _costOfAnvilTrap;
	}
	else if (_toPlace._subType == TESLACOIL)
	{
		_toPlace._goldCost = _costOfTeslaCoil;
	}

	if (_toPlace._type == CAMERA)
	{
		_toPlace._goldCost = _costOfCamera;
	}
	else if (_toPlace._type == GUARD)
	{
		_toPlace._goldCost = _costOfGuard;
	}
}

PlacementState::~PlacementState()
{
	delete _baseEdit;
	_baseEdit = nullptr;
	delete _tutorialLogic;
	_tutorialLogic = nullptr;
}

void PlacementState::Update(float deltaTime)
{
	//if tutorial mode. Then bypass normal baseEdit update loops.
	if (_tutorialStage != TutorialStage::NOTUTORIAL)
	{
		//bypass the normal UI interface to interface the tutorial elements into it.
		_tutorialLogic->Update(deltaTime, _baseEdit, _toPlace, _playerProfile.at(_currentPlayer));
		if (_tutorialLogic->IsTutorialCompleted())
		{
			ChangeState(State::PLAYSTATE);
			_tutorialStage = TutorialStage::NOTUTORIAL;
		}
	}
	else
	{
		//Handle the buttons normally
		HandleButtons();
	}
	HandleInput();
	
	
	//baseEdit update handles basic controls.
	_baseEdit->Update(deltaTime);
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
	_baseEdit = new BaseEdit(_objectHandler, _controls, _pickingDevice, _camera);
	_objectHandler->DisableSpawnPoints();

	XMFLOAT3 campos;
	campos.x = _objectHandler->GetTileMap()->GetWidth() / 2;
	campos.y = 15;
	campos.z = _objectHandler->GetTileMap()->GetHeight() / 2 - 10;
	_camera->SetPosition(campos);

	//if tutorial is active but we are pausing. Hide tutorial.
	if (_tutorialStage != TutorialStage::NOTUTORIAL)
	{
		_uiTree.GetNode("Tutorial")->SetHidden(false);
	}
	//if it is also New tutorial. Reset it so we can start a new.
	if (_tutorialStage != TutorialStage::NEWTUTORIAL)
	{
		_tutorialLogic->ResetUiTree();
	}
}

void PlacementState::OnStateExit()
{
	delete _baseEdit;
	_baseEdit = nullptr;
	//if the tutorialstage is anything other than no tutorial. Hide it. We want to reset on entry, not exit.
	if (_tutorialStage != NOTUTORIAL)
	{
		_uiTree.GetNode("Tutorial")->SetHidden(true);
	}
}

void PlacementState::HandleInput()
{
	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		ChangeState(PAUSESTATE);
	}

	_baseEdit->DragAndDrop(TRAP);
	_baseEdit->DragAndDrop(GUARD);
	_baseEdit->DragAndDrop(CAMERA);

	if (_baseEdit->GetSelectedObject() != nullptr)
	{
		_toPlace._type = _baseEdit->GetSelectedObject()->GetType();
		_toPlace._subType =_baseEdit->GetSelectedObject()->GetSubType();
		_toPlace._blueprintID = _baseEdit->GetSelectedObject()->GetID();
		EvaluateGoldCost();
		if (_controls->IsFunctionKeyDown("MAP_EDIT:DELETE_UNIT"))
		{
			_playerProfile[_currentPlayer]._gold += _toPlace._goldCost;
			_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));
			_baseEdit->DeleteSelectedObject();
			_toPlace.ResetTemps();
		}
	}

	// placement invalid
	if (_toPlace._goldCost != -1 && !_objectHandler->Find(_toPlace._type, _toPlace._blueprintID) && !_baseEdit->IsPlace())
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

	/*
	//Tutorial image
	if (_uiTree.IsButtonColliding("Tutorial", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		//Hide how to screen when clicked
		_uiTree.GetNode("Tutorial")->SetHidden(true);
		_playerProfile[_currentPlayer]._firstTime = false;
		System::saveJSON(&_playerProfile[_currentPlayer], _playerProfilesPath[_currentPlayer], "Player Profile");
	}
	*/

	//Play
	if (_uiTree.IsButtonColliding("Play", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		ChangeState(PLAYSTATE);
	}

	//Item UI
	if (_uiTree.IsButtonColliding("Guard", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		// Temp, should be replaced with blueprint
		_toPlace._type = GUARD;
		_toPlace._name = "guard_proto";

		if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
		{
			create = true;
		}
	}

	if (_uiTree.IsButtonColliding("AnvilTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		// Temp, should be replaced with blueprint
		_toPlace._type = TRAP;
		_toPlace._name = "trap_proto";

		if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
		{
			_toPlace._subType = SPIKE;
			create = true;
		}
	}
	if (_uiTree.IsButtonColliding("TeslaTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		_toPlace._type = TRAP;
		_toPlace._name = "tesla_trap";

		if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
		{
			_toPlace._subType = TESLACOIL;
			create = true;
		}
	}
	if (_uiTree.IsButtonColliding("Camera", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		_toPlace._type = CAMERA;
		_toPlace._name = "camera_proto";

		if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
		{
			create = true;
		}
	}

	//Check whether we can afford to place items.
	if (create)
	{
		EvaluateGoldCost();

		if (_playerProfile[_currentPlayer]._gold >= _toPlace._goldCost)
		{
			_baseEdit->DragActivate(_toPlace._type, _toPlace._name, _toPlace._subType);
			_playerProfile[_currentPlayer]._gold -= _toPlace._goldCost;
			_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));
			_toPlace._blueprintID = _baseEdit->GetSelectedObject()->GetID();
		}
		else
		{
			_toPlace._goldCost = -1;
		}
	}

//	System::saveJSON(&_playerProfile[_currentPlayer], _playerProfilesPath[_currentPlayer], "Player Profile");

	//if (_uiTree.IsButtonColliding("Camera", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	//{
	//	
	//}

}


