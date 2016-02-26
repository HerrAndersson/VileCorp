#include "PlacementState.h"

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule),
	_ghostImage(objectHandler, pickingDevice)
{
	_playerProfile.resize(1);
	_tutorialLogic = new TutorialLogic(&_uiTree, _controls);
	_player = new Player(_objectHandler);
	_buttons = _uiTree.GetNode("UnitList")->GetChildren();

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
	_ambientLight = ambientLight;

	//Add sound
	_soundModule->AddSound("in_game_1", 0.2f, 1.0f, true, true);
}

void PlacementState::EvaluateGoldCost()
{
}

PlacementState::~PlacementState()
{
	delete _player;
	_player = nullptr;
	delete _tutorialLogic;
	_tutorialLogic = nullptr;
}

void PlacementState::Update(float deltaTime)
{
	System::MouseCoord coord = _controls->GetMouseCoord();
	//if tutorial mode. Then bypass normal baseEdit update loops.
	if (_tutorialState != TutorialState::NOTUTORIAL)
	{
		//bypass the normal UI interface to interface the tutorial elements into it.
		//_tutorialLogic->Update(deltaTime, _baseEdit, _toPlace, _playerProfile.at(_currentPlayer));
		if (_tutorialLogic->IsTutorialCompleted())
		{
			ChangeState(State::PLAYSTATE);
			_tutorialState = TutorialState::NOTUTORIAL;
		}
	}
	HandleDescriptions();
	HandleButtonHighlight(coord);
	HandleInput();
	HandleCam(deltaTime);
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
	_objectHandler->DisableSpawnPoints();

	XMFLOAT3 campos;
	campos.x = _objectHandler->GetTileMap()->GetWidth() / 2;
	campos.y = 15;
	campos.z = _objectHandler->GetTileMap()->GetHeight() / 2 - 10;
	_camera->SetPosition(campos);

	_buttonHighlights.clear();
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("Play")));

	//std::vector<GUI::Node*>* units = _uiTree.GetNode("UnitList")->GetChildren();
	//for (int i = 0; i < units->size(); i++)
	//{
	//	GUI::BlueprintNode* newUnit = new GUI::BlueprintNode(*units->at(i), _objectHandler->GetBlueprintByName(units->at(i)->GetId()), 0);
	//	delete units->at(i);
	//	units->at(i) = (GUI::Node*)newUnit;
	//	_buttonHighlights.push_back(GUI::HighlightNode(units->at(i), XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f)));
	//}

	//TODO: Move hardcoded costs and description to logical location /Rikhard

	_uiTree.GetNode("GuardDescription")->SetHidden(true);
	_uiTree.GetNode("GuardCost")->SetText(L"Cost: " + to_wstring(200) + L"$");
	_uiTree.GetNode("AnvilDescription")->SetHidden(true);
	_uiTree.GetNode("AnvilCost")->SetText(L"Cost: " + to_wstring(50) + L"$");
	_uiTree.GetNode("TeslaDescription")->SetHidden(true);
	_uiTree.GetNode("TeslaCost")->SetText(L"Cost: " + to_wstring(100) + L"$");
	_uiTree.GetNode("MachineGunDescription")->SetHidden(true);
	_uiTree.GetNode("MachineGunCost")->SetText(L"Cost: " + to_wstring(120) + L"$");
	//_uiTree.GetNode("SharkDescription")->SetHidden(true);
	//_uiTree.GetNode("SharkCost")->SetText(L"Cost: " + to_wstring(150) + L"$");
	_uiTree.GetNode("CameraDescription")->SetHidden(true);
	_uiTree.GetNode("CameraCost")->SetText(L"Cost: " + to_wstring(80) + L"$");

	if (_tutorialState == TutorialState::NEWTUTORIAL)
	{
		_uiTree.GetNode("Tutorial")->SetHidden(false);
		_tutorialLogic->ResetUiTree();
	}
	//Coming back from pause state
	else if (_tutorialState == TutorialState::OLDTUTORIAL)
	{
		_uiTree.GetNode("Tutorial")->SetHidden(false);
	}
	else if (_tutorialState == TutorialState::NOTUTORIAL)
	{
		_uiTree.GetNode("Tutorial")->SetHidden(true);
	}

	//Play music
	_soundModule->Play("in_game_1");
}

void PlacementState::OnStateExit()
{
	_player->DeselectUnits();
	//if the tutorialstage is anything other than no tutorial. Hide it. We want to reset on entry, not exit.
	if (_tutorialState != NOTUTORIAL)
	{
		_uiTree.GetNode("Tutorial")->SetHidden(true);
	}

	//Pause music
	_soundModule->Pause("in_game_1");
}

void PlacementState::HandleInput()
{
	System::MouseCoord coord = _controls->GetMouseCoord();
	_ghostImage.Update(coord);

	//Menu
	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		if (_ghostImage.IsGhostImageActive())
		{
			_ghostImage.RemoveGhostImage();
		}
		else
		{
			ChangeState(PAUSESTATE);
		}
	}
	//Play
	if (_uiTree.IsButtonColliding("Play", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		ChangeState(PLAYSTATE);
	}

	//Left Click
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		//Check if we click on any of the many buttons or rest of scene
		bool hit = false;

		//Button interaction. Select a blueprint
		for (int i = 0; i < _buttons->size(); i++)
		{
			GUI::Node* currentButton = _buttons->at(i);
			// if we click a button. Load itemspecifics into _selectedBlueprint
			if (_uiTree.IsButtonColliding(currentButton, coord._pos.x, coord._pos.y))
			{
				GUI::BlueprintNode* currentBlueprintButton = static_cast<GUI::BlueprintNode*>(currentButton);
				_selectedBlueprint._blueprint = currentBlueprintButton->GetBlueprint();
				_selectedBlueprint._textureId = currentBlueprintButton->GetTextureId();
				//TODO Add ghostimage /Alex
				_ghostImage.AddGhostImage(_selectedBlueprint, coord);
				hit = true;
				break;
			}
		}
		//if we dont hit the buttons - Clicking on the rest of the scene
		if (!hit)
		{
			//if we already hit the button. We use the blueprint
			if (_selectedBlueprint._blueprint != nullptr)
			{
				//Try if/and then place item
				AI::Vec2D pickedTile = _pickingDevice->PickTile(coord._pos);
				DirectX::XMFLOAT3 pos = XMFLOAT3(pickedTile._x, 0, pickedTile._y);
				if (_objectHandler->Add(_selectedBlueprint._blueprint, _selectedBlueprint._textureId, pos, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), true))
				{
					_ghostImage.RemoveGhostImage();
					//Object Added. Calculate money and deselect object TODO: Look over where money is stored and use from correct path /Alex
					int temp = _budget;// -_selectedBlueprint._blueprint->_money;
					if (temp > 0)
					{
						_budget = temp;
						_selectedBlueprint.Reset();
					}
				}
			}
			//Selecting/Deselecting objects in the scene
			else
			{
				//Selecting placed units
				_pickingDevice->SetFirstBoxPoint(_controls->GetMouseCoord()._pos);
			}
		}
	}

	//Left click up
	if (_controls->IsFunctionKeyUp("MOUSE:SELECT") && _selectedBlueprint._blueprint == nullptr)
	{
		//Deselect everything first by first remove the color of the objects and then deselecting it.
		vector<GameObject*> deselectObjects = _player->GetSelectedObjects();

		//Decolourize everything
		for (GameObject* i : deselectObjects)
		{
			i->SetColorOffset(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		}
		_player->DeselectObjects();

		//Check if we picked anything
		vector<vector<GameObject*>> pickedUnits;
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(GUARD)));
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(TRAP)));
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(CAMERA)));

		//Then Select it
		_player->SelectObjects(pickedUnits);

		//Retrieve the objects in an actual container and mark them with colour
		vector<GameObject*> objects = _player->GetSelectedObjects();
		for (GameObject* i : objects)
		{
			i->SetColorOffset(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		}
	}

	/*
	//Left click up
	if (_controls->IsFunctionKeyUp("MOUSE:SELECT") && _selectedBlueprint._blueprint == nullptr)
	{
		//Deselect everything first by first remove the color of the objects and then deselecting it.
		vector<GameObject*> deselectObjects = _player->GetSelectedObjects();

		//Decolourize everything
		for (GameObject* i : deselectObjects)
		{
			i->SetColorOffset(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		}
		_player->DeselectObjects();

		//Check if we picked anything
		vector<vector<GameObject*>> pickedUnits;
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(GUARD)));
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(TRAP)));
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(CAMERA)));

		//Then Select it
		_player->SelectObjects(pickedUnits);

		//Retrieve the objects in an actual container and mark them with colour
		vector<GameObject*> objects = _player->GetSelectedObjects();
		for (GameObject* i : objects)
		{
			i->SetColorOffset(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		}
	}*/

	/*
	//Left click - placing units - Blueprint specific - i.e. click button and placing object.
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		//If we dont have a blue print selected
		if (_selectedBlueprint._blueprint == nullptr)
		{
			//Button interaction. Select a blueprint
			std::vector<GUI::Node*>* units = _uiTree.GetNode("UnitList")->GetChildren();
			for (int i = 0; i < units->size(); i++)
			{
				GUI::Node* currentButton = units->at(i);
				// if we click a button. Load itemspecifics into _selectedBlueprint
				if (_uiTree.IsButtonColliding(currentButton, coord._pos.x, coord._pos.y))
				{
					GUI::BlueprintNode* currentBlueprintButton = static_cast<GUI::BlueprintNode*>(currentButton);
					_selectedBlueprint._blueprint = currentBlueprintButton->GetBlueprint();
					_selectedBlueprint._textureId = currentBlueprintButton->GetTextureId();
					//TODO Add ghostimage /Alex
					break;
				}
			}
		}
		//else we already have a blueprint selected
		else
		{
			//Try if/and then place item
			AI::Vec2D pickedTile = _pickingDevice->PickTile(coord._pos);
			DirectX::XMFLOAT3 pos = XMFLOAT3(pickedTile._x, 0, pickedTile._y);
			if (_objectHandler->Add(_selectedBlueprint._blueprint, _selectedBlueprint._textureId, pos, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), true))
			{
				//Object Added. Calculate money and deselect object TODO: Look over where money is stored and use from correct path /Alex
				int temp = _budget;// -_selectedBlueprint._blueprint->_money;
				if (temp > 0)
				{
					_budget = temp;
					_selectedBlueprint.Reset();
				}
			}
		}
	}*/
	/*
	//Selecting placed units
	//If left click to select units(drag box)
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT") && _selectedBlueprint._blueprint == nullptr)
	{
		_pickingDevice->SetFirstBoxPoint(_controls->GetMouseCoord()._pos);
	}
	*/
	/*
	//Left click up
	if (_controls->IsFunctionKeyUp("MOUSE:SELECT") && _selectedBlueprint._blueprint == nullptr)
	{
		//Deselect everything first by first remove the color of the objects and then deselecting it.
		vector<GameObject*> deselectObjects = _player->GetSelectedObjects();

		//Decolourize everything
		for (GameObject* i : deselectObjects)
		{
			i->SetColorOffset(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		}
		_player->DeselectObjects();

		//Check if we picked anything
		vector<vector<GameObject*>> pickedUnits;
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(GUARD)));
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(TRAP)));
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(CAMERA)));

		//Then Select it
		_player->SelectObjects(pickedUnits);

		//Retrieve the objects in an actual container and mark them with colour
		vector<GameObject*> objects = _player->GetSelectedObjects();
		for (GameObject* i : objects)
		{
			i->SetColorOffset(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		}
	}
	*/

	//Deselect blueprint
	if (_controls->IsFunctionKeyDown("MOUSE:DESELECT"))
	{
		//If we have a selected unit and want to deselect
		if (_selectedBlueprint._blueprint != nullptr)
		{
			_selectedBlueprint.Reset();
		}
		_player->DeselectObjects();
	}

	//Delete object
	if (_controls->IsFunctionKeyDown("MAP_EDIT:DELETE_UNIT"))
	{
		vector<GameObject*> objectsToDelete = _player->GetSelectedObjects();
		if (objectsToDelete.size() > 0)
		{
			for (GameObject* i : objectsToDelete)
			{
				_objectHandler->Remove(i);
			}
		}
		_player->DeselectObjects();
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
	/*
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
	*/
	// placement invalid
	//if (_toPlace._goldCost != -1 && !_objectHandler->Find(_toPlace._sB._blueprint->_type, _toPlace._markerID) && !_baseEdit->IsPlace())
	//{
	//	_budget += _toPlace._goldCost;
	//	_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_budget));
	//	_toPlace.ResetTemps();
	//}
}

	/*
	if (create)
	{
		EvaluateGoldCost();
		_baseEdit->HandleBlueprint(&_toPlace._sB);
		_toPlace._blueprintID = _baseEdit->GetMarkedObject()->GetID();

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
	*/

	//Item UI
	//if (_uiTree.IsButtonColliding("Guard", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	//{
	//	// Temp, should be replaced with blueprint
	//	_toPlace._type = GUARD;
	//	_toPlace._name = "guard_proto";

	//	if (_baseEdit->IsSelection() && !_baseEdit->IsPlace())
	//	{
	//		create = true;
	//	}
	//}

void PlacementState::HandleDescriptions()
{
	System::MouseCoord coord = _controls->GetMouseCoord();
	if (_uiTree.IsButtonColliding("Guard", coord._pos.x, coord._pos.y))
	{
		_uiTree.GetNode("GuardDescription")->SetHidden(false);

		// Add description
	}
	else
	{
		_uiTree.GetNode("GuardDescription")->SetHidden(true);
	}

	if (_uiTree.IsButtonColliding("AnvilTrap", coord._pos.x, coord._pos.y))
	{
		_uiTree.GetNode("AnvilDescription")->SetHidden(false);

		// Add description
	}
	else
	{
		_uiTree.GetNode("AnvilDescription")->SetHidden(true);
	}

	if (_uiTree.IsButtonColliding("TeslaTrap", coord._pos.x, coord._pos.y))
	{
		_uiTree.GetNode("TeslaDescription")->SetHidden(false);

		// Add description
	}
	else
	{
		_uiTree.GetNode("TeslaDescription")->SetHidden(true);
	}

	if (_uiTree.IsButtonColliding("MachineGunTrap", coord._pos.x, coord._pos.y))
	{
		_uiTree.GetNode("MachineGunDescription")->SetHidden(false);

		// Add description
	}
	else
	{
		_uiTree.GetNode("MachineGunDescription")->SetHidden(true);
	}

	//if (_uiTree.IsButtonColliding("SharkTrap", coord._pos.x, coord._pos.y))
	//{
	//	_uiTree.GetNode("SharkDescription")->SetHidden(false);

	//	// Add description
	//}
	//else
	//{
	//	_uiTree.GetNode("SharkDescription")->SetHidden(true);
	//}

	if (_uiTree.IsButtonColliding("Camera", coord._pos.x, coord._pos.y))
	{
		_uiTree.GetNode("CameraDescription")->SetHidden(false);

		// Add description
	}
	else
	{
		_uiTree.GetNode("CameraDescription")->SetHidden(true);
	}
}