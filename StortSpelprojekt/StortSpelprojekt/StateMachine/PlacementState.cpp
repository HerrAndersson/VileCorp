#include "PlacementState.h"

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "PLACEMENT", assetManager, fontWrapper, settings, soundModule)
{
	_playerProfile.resize(1);
	
	
	//load all player profiles
	GetFilenamesInDirectory("Assets/PlayerProfiles/", ".json", _playerProfilesPath);
	
	//rezise vector that stores player profiles
	_playerProfile.resize(_playerProfilesPath.size());

	for (int i = 0; i < _playerProfilesPath.size(); i++)
	{

		System::loadJSON(&_playerProfile[i], _playerProfilesPath[i]);
	}

	
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
	//TODO: Move this function to LevelSelection when that state is created. /Alex
	_objectHandler->LoadLevel(9);

	_baseEdit = new BaseEdit(_objectHandler, _controls, _pickingDevice, _camera);
	_objectHandler->DisableSpawnPoints();

	XMFLOAT3 campos;
	campos.x = _objectHandler->GetTileMap()->GetWidth() / 2;
	campos.y = 15;
	campos.z = _objectHandler->GetTileMap()->GetHeight() / 2 - 10;
	_camera->SetPosition(campos);

	if (_playerProfile[0]._firstTime != true)
	{
		_uiTree.GetNode("Tutorial")->SetHidden(true);
	}

	

}

void PlacementState::OnStateExit()
{
	delete _baseEdit;
	_baseEdit = nullptr;
}

void PlacementState::HandleInput()
{
	_baseEdit->DragAndDrop(TRAP);
	_baseEdit->DragAndDrop(GUARD);
	_baseEdit->DragAndDrop(CAMERA);

	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		ChangeState(PAUSESTATE);
	}

	if (_controls->IsFunctionKeyDown("MAP_EDIT:DELETE_UNIT"))
	{
		if (_baseEdit->GetSelectedObject() != nullptr)
		{
			_playerProfile[_currentPlayer]._gold += 10;
			_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));
			_baseEdit->DeleteSelectedObject();
		}
	}
}

void PlacementState::HandleButtons()
{
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

	if (_uiTree.IsButtonColliding("AnvilTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT") && _playerProfile[_currentPlayer]._gold >= 10)
	{
		// Temp, should be replaced with blueprint
		_toPlace._type = TRAP;
		_toPlace._name = "trap_proto";

		if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
		{
			_baseEdit->DragActivate(_toPlace._type, _toPlace._name, SPIKE);

			_playerProfile[_currentPlayer]._gold -= 10;
			_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));

			//C:/dev/StortSpel-Grupp1/Output/Bin/x86/Debug/

		}
	}
	if (_uiTree.IsButtonColliding("TeslaTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT") && _playerProfile[_currentPlayer]._gold >= 100)
	{
		_toPlace._type = TRAP;
		_toPlace._name = "tesla_trap";

		if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
		{
			_baseEdit->DragActivate(_toPlace._type, _toPlace._name, TESLACOIL);
			
			_playerProfile[_currentPlayer]._gold -= 100;
			_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));
		}
	}
	if (_uiTree.IsButtonColliding("Camera", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT") && _playerProfile[_currentPlayer]._gold >= 80)
	{
		_toPlace._type = CAMERA;
		_toPlace._name = "camera_proto";

		if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
		{
			_baseEdit->DragActivate(_toPlace._type, _toPlace._name);
			_playerProfile[_currentPlayer]._gold -= 80;
			_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));
		}
	}

	if (_uiTree.IsButtonColliding("Guard", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT") && _playerProfile[_currentPlayer]._gold >= 250)
	{
		// Temp, should be replaced with blueprint
		_toPlace._type = GUARD;
		_toPlace._name = "guard_proto";

		if (_baseEdit->IsSelection())
		{
			_baseEdit->DragActivate(_toPlace._type, _toPlace._name);
			
			_playerProfile[_currentPlayer]._gold -= 250;
			_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_playerProfile[_currentPlayer]._gold));
			
		}
	}
//	System::saveJSON(&_playerProfile[_currentPlayer], _playerProfilesPath[_currentPlayer], "Player Profile");

	//if (_uiTree.IsButtonColliding("Camera", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	//{
	//	
	//}

}


