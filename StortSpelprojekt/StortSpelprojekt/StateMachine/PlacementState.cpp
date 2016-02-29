#include "PlacementState.h"

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule),
	_ghostImage(objectHandler, pickingDevice)
{
	_player = new Player(_objectHandler, pickingDevice);
	_buttons = _uiTree.GetNode("UnitList")->GetChildren();
	_profile = settingsReader->GetProfile();
	_tutorialLogic = new TutorialLogic(&_uiTree, _controls, _player, _buttons, &_ghostImage, objectHandler, pickingDevice, _profile);

	//Money
	_costOfAnvilTrap	= 50;
	_costOfTeslaCoil	= 100;
	_costOfCamera		= 80;
	_costOfGuard		= 200;

	_controls = controls;
	_objectHandler = objectHandler;
	_camera = camera;
	_pickingDevice = pickingDevice;
	_ambientLight = ambientLight;

	//Add sound
	_soundModule->AddSound("in_game_1", 0.2f, 1.0f, true, true, true);
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
		_tutorialLogic->Update(deltaTime);
		if (_tutorialLogic->IsTutorialCompleted())
		{
			ChangeState(State::PLAYSTATE);
			_tutorialState = TutorialState::NOTUTORIAL;
		}

		//Menu - We keep this outside of tutorial due to the changestate function.
		if (_controls->IsFunctionKeyDown("MENU:MENU"))
		{
			if (_ghostImage.IsGhostImageActive() || _player->IsSelectedObjects())
			{
				_ghostImage.RemoveGhostImage();
				_player->DeselectObjects();
			}
			else
			{
				ChangeState(PAUSESTATE);
			}
		}
	}
	//else Normal games
	else
	{
		HandleInput();
	}
	HandleDescriptions();
	HandleButtonHighlight(coord);
	HandleCam(deltaTime);
}

void PlacementState::OnStateEnter()
{
	_ambientLight->x = AMBIENT_LIGHT_DAY.x;
	_ambientLight->y = AMBIENT_LIGHT_DAY.y;
	_ambientLight->z = AMBIENT_LIGHT_DAY.z;

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
		if (_ghostImage.IsGhostImageActive() || _player->IsSelectedObjects())
		{
			_ghostImage.RemoveGhostImage();
			_player->DeselectObjects();
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
		bool hitButtons = false;

		//Button interaction. Select a blueprint
		for (int i = 0; i < _buttons->size(); i++)
		{
			GUI::Node* currentButton = _buttons->at(i);
			// if we click a button. Load itemspecifics into _selectedBlueprint
			if (_uiTree.IsButtonColliding(currentButton, coord._pos.x, coord._pos.y))
			{
				GUI::BlueprintNode* currentBlueprintButton = static_cast<GUI::BlueprintNode*>(currentButton);
				_selectedBlueprint._blueprint = _objectHandler->GetBlueprintByType(currentBlueprintButton->GetType(), currentBlueprintButton->GetSubType());
				_selectedBlueprint._textureId = currentBlueprintButton->GetTextureId();
				_ghostImage.AddGhostImage(_selectedBlueprint, coord);
				hitButtons = true;
				break;
			}
		}
		//if we dont hit the buttons - Clicking on the rest of the scene
		if (!hitButtons)
		{
			AI::Vec2D pickedTile = _pickingDevice->PickTile(coord._pos);
			//if we already hit the button. We use the blueprint
			if (_selectedBlueprint._blueprint != nullptr)
			{
				//Try if/and then place item
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
			//else we are Selecting objects in the scene or dragging
			else
			{
				//Selecting placed units
				_pickingDevice->SetFirstBoxPoint(_controls->GetMouseCoord()._pos);

				//Do we have anything selected if so. We will check if we want to drag or deselect
				if (_player->IsSelectedObjects())
				{
					//Check the selected objects, if we click on one of them. Then activate dragging with it.
					vector<GameObject*> hitObjects = _pickingDevice->PickObjects(coord._pos, _player->GetSelectedObjects());
					if (!hitObjects.empty())
					{
						_player->ActivateDragging(hitObjects[0]);
					}
				}
			}
		}
	}

	//Drag update
	_player->UpdateDragPositions(coord);

	//Left click up
	if (_controls->IsFunctionKeyUp("MOUSE:SELECT") && _selectedBlueprint._blueprint == nullptr)
	{
		//Deselect everything first by first remove the color of the objects and then deselecting it.
		_player->DeactivateDragging();
		_player->DeselectObjects();

		//Check if we picked anything
		vector<vector<GameObject*>> pickedUnits;
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(System::GUARD)));
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(System::TRAP)));
		pickedUnits.push_back(_pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(System::CAMERA)));

		//Then Select it
		_player->SelectObjects(pickedUnits);
	}

	//Rotation
	//Rotation With Keys
	if (_controls->IsFunctionKeyUp("MAP_EDIT:ROTATE_MARKER_COUNTERCLOCK"))
	{
		vector<GameObject*> objects = _player->GetSelectedObjects();
		XMFLOAT3 tempRot;
		for (GameObject* i : objects)
		{
			tempRot = i->GetRotation();
			i->SetRotation(XMFLOAT3(tempRot.x, tempRot.y - (DirectX::XM_PI / 4), tempRot.z));
		}

	}
	else if (_controls->IsFunctionKeyUp("MAP_EDIT:ROTATE_MARKER_CLOCK"))
	{
		vector<GameObject*> objects = _player->GetSelectedObjects();
		XMFLOAT3 tempRot;
		for (GameObject* i : objects)
		{
			tempRot = i->GetRotation();
			i->SetRotation(XMFLOAT3(tempRot.x, tempRot.y + (DirectX::XM_PI / 4), tempRot.z));
		}
	}

	//Deselect blueprint
	if (_controls->IsFunctionKeyDown("MOUSE:DESELECT"))
	{
		//If we have a selected unit and want to deselect it.
		if (_selectedBlueprint._blueprint != nullptr)
		{
			_selectedBlueprint.Reset();
			_ghostImage.RemoveGhostImage();
		}
		else
		{
			//Rotation with mouse
			//if one object and clicking on its tile
			vector<GameObject*> objects = _player->GetSelectedObjects();
			if (1 == objects.size())
			{
				//if (objects[0]->GetTilePosition() == _pickingDevice->PickTile(coord._pos))
				//{
					//Check which direction he should be pointing
					AI::Vec2D direction = _pickingDevice->PickDirection(coord._pos, objects[0]->GetTilePosition(), _objectHandler->GetTileMap());

					//Change direction
					vector<GameObject*> objects = _player->GetSelectedObjects();
					for (GameObject* i : objects)
					{
						i->SetDirection(direction);
					}
				//}
			}
			else
			{
				_player->DeselectObjects();
			}
		}
	}

	//Delete object
	if (_controls->IsFunctionKeyDown("MAP_EDIT:DELETE_UNIT"))
	{
		_player->DeleteSelectedObjects();
	}
}

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