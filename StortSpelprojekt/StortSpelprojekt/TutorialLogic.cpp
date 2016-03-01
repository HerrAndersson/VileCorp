#include "TutorialLogic.h"

TutorialLogic::TutorialLogic(GUI::UITree* uiTree, System::Controls* controls, Player* player, std::vector<GUI::Node*>* buttons, BaseEdit* baseEdit, ObjectHandler* objectHandler, PickingDevice* pickingDevice, System::Profile* profile)
{
	_uiTree = uiTree;
	_controls = controls;
	_player = player;
	_buttons = buttons;
	_baseEdit = baseEdit;
	_objectHandler = objectHandler;
	_pickingDevice = pickingDevice;
	_profile = profile;
	_budget = 500;
	_sCameraPlaced = false;
	_tutorialCompleted = false;
	ResetUiTree();
	_time = 0;
	_light = false;
	_objectPlaced = false;
}

TutorialLogic::~TutorialLogic()
{
	_uiTree->GetNode("welcome")->SetHidden(true);
	_uiTree->GetNode("controls")->SetHidden(true);
	_uiTree->GetNode("objective")->SetHidden(true);
	_uiTree->GetNode("guardexplained")->SetHidden(true);
	_uiTree->GetNode("guardplace")->SetHidden(true);
	_uiTree->GetNode("anvilexplained")->SetHidden(true);
	_uiTree->GetNode("anvilplace")->SetHidden(true);
	_uiTree->GetNode("teslaexplained")->SetHidden(true);
	_uiTree->GetNode("teslaplace")->SetHidden(true);
	_uiTree->GetNode("securitycameraexplained")->SetHidden(true);
	_uiTree->GetNode("securitycameraplace")->SetHidden(true);
	_uiTree->GetNode("budgetexplained")->SetHidden(true);
	_uiTree->GetNode("playexplained")->SetHidden(true);
}

void TutorialLogic::ResetUiTree()
{
	_tutorialCompleted = false;
	_currentStage = WELCOME;
	_uiTree->GetNode("welcome")->SetHidden(false);
	_uiTree->GetNode("controls")->SetHidden(true);
	_uiTree->GetNode("objective")->SetHidden(true);
	_uiTree->GetNode("guardexplained")->SetHidden(true);
	_uiTree->GetNode("guardplace")->SetHidden(true);
	_uiTree->GetNode("anvilexplained")->SetHidden(true);
	_uiTree->GetNode("anvilplace")->SetHidden(true);
	_uiTree->GetNode("teslaexplained")->SetHidden(true);
	_uiTree->GetNode("teslaplace")->SetHidden(true);
	_uiTree->GetNode("securitycameraexplained")->SetHidden(true);
	_uiTree->GetNode("securitycameraplace")->SetHidden(true);
	_uiTree->GetNode("budgetexplained")->SetHidden(true);
	_uiTree->GetNode("playexplained")->SetHidden(true);
}

bool TutorialLogic::Update(float deltaTime)
{
	System::MouseCoord coord = _controls->GetMouseCoord();
	_time += deltaTime;
	//icon blink speed
	if (_time > 800)
	{
		_time = 0;
		_light = !_light;
	}
	
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree->ReloadTree("../../../../StortSpelprojekt/Assets/GUI/placement.json");
		ResetUiTree();
	}

	switch (_currentStage)
	{
	case WELCOME:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = CONTROLS;
			_uiTree->GetNode("welcome")->SetHidden(true);
			_uiTree->GetNode("controls")->SetHidden(false);
		}
		break;
	}
	case CONTROLS:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = OBJECTIVE;
			_uiTree->GetNode("controls")->SetHidden(true);
			_uiTree->GetNode("objective")->SetHidden(false);
		}
		break;
	}
	case OBJECTIVE:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = GUARDEXPLAINED;
			_uiTree->GetNode("objective")->SetHidden(true);
			_uiTree->GetNode("guardexplained")->SetHidden(false);
		}
		break;
	}
	
	case GUARDEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = GUARDPLACE;
			_uiTree->GetNode("guardexplained")->SetHidden(true);
			_uiTree->GetNode("guardplace")->SetHidden(false);
		}
		break;
	}
	case GUARDPLACE:
	{
		//Blink button
		if (_light)
		{
			_uiTree->GetNode("Guard")->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			_uiTree->GetNode("Guard")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		
		InputButtons(System::GUARD, SUBTYPEGUARD);
		//if player managed to place the object. We switch state and relevant values to default.
		if (_objectPlaced)
		{

			_currentStage = ANVILEXPLAINED;
			_objectPlaced = false;
			_uiTree->GetNode("guardplace")->SetHidden(true);
			_uiTree->GetNode("anvilexplained")->SetHidden(false);
			_uiTree->GetNode("Guard")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		break;
	}
	case ANVILEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = ANVILPLACE;
			_uiTree->GetNode("anvilexplained")->SetHidden(true);
			_uiTree->GetNode("anvilplace")->SetHidden(false);
		}
		break;
	}
	case ANVILPLACE:
	{
		//Blink button
		if (_light)
		{
			_uiTree->GetNode("AnvilTrap")->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			_uiTree->GetNode("AnvilTrap")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		InputButtons(System::TRAP, SUBTYPESPIKE);
		//if player managed to place the object. We switch state and relevant values to default.
		if (_objectPlaced)
		{
			_currentStage = TESLAEXPLAINED;
			_objectPlaced = false;
			_uiTree->GetNode("anvilplace")->SetHidden(true);
			_uiTree->GetNode("teslaexplained")->SetHidden(false);
			_uiTree->GetNode("AnvilTrap")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		break;
	}
	case TESLAEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = TESLAPLACE;
			_uiTree->GetNode("teslaexplained")->SetHidden(true);
			_uiTree->GetNode("teslaplace")->SetHidden(false);
		}
		break;
	}
	case TESLAPLACE:
	{
		//Blink button
		if (_light)
		{
			_uiTree->GetNode("TeslaTrap")->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			_uiTree->GetNode("TeslaTrap")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}

		InputButtons(System::TRAP, SUBTYPETESLACOIL);
		//if player managed to place the object. We switch state and relevant values to default.
		if (_objectPlaced)
		{
			_currentStage = SECURITYCAMERAEXPLAINED;
			_objectPlaced = false;
			_uiTree->GetNode("teslaplace")->SetHidden(true);
			_uiTree->GetNode("securitycameraexplained")->SetHidden(false);
			_uiTree->GetNode("TeslaTrap")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		break;
	}
	case SECURITYCAMERAEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = SECURITYCAMERAPLACE;
			_uiTree->GetNode("securitycameraexplained")->SetHidden(true);
			_uiTree->GetNode("securitycameraplace")->SetHidden(false);
		}
		break;
	}
	case SECURITYCAMERAPLACE:
	{
		//Blink button
		if (_light)
		{
			_uiTree->GetNode("Camera")->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			_uiTree->GetNode("Camera")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		InputButtons(System::CAMERA, 0);
		//if player managed to place the object. We switch state and relevant values to default.
		if (_objectPlaced)
		{
			_uiTree->GetNode("Camera")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE") && _objectPlaced)
		{
			_currentStage = BUDGETEXPLAINED;
			_objectPlaced = false;
			_uiTree->GetNode("securitycameraplace")->SetHidden(true);
			_uiTree->GetNode("budgetexplained")->SetHidden(false);
			_uiTree->GetNode("Camera")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		break;
	}
	case BUDGETEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = PLAYEXPLAINED;
			_uiTree->GetNode("budgetexplained")->SetHidden(true);
			_uiTree->GetNode("playexplained")->SetHidden(false);
		}

		break;
	}
	case PLAYEXPLAINED:
	{
		//Blink button
		if (_light)
		{
			_uiTree->GetNode("Play")->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			_uiTree->GetNode("Play")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		InputButtons();
		if (_uiTree->IsButtonColliding("Play", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			_profile->_firstTime = false;
			_tutorialCompleted = true;
			_uiTree->GetNode("Play")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		break;
	}
	default:
		break;
	}
	
	return _tutorialCompleted;
}

void TutorialLogic::InputButtons(System::Type type, int subType)
{
	System::MouseCoord coord = _controls->GetMouseCoord();
	_ghostImage->Update(coord);

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
			if (_uiTree->IsButtonColliding(currentButton, coord._pos.x, coord._pos.y))
			{
				GUI::BlueprintNode* currentBlueprintButton = static_cast<GUI::BlueprintNode*>(currentButton);
				if (currentBlueprintButton->GetType() == type && currentBlueprintButton->GetSubType() == subType)
				{
					_selectedBlueprint._blueprint = _objectHandler->GetBlueprintByType(currentBlueprintButton->GetType(), currentBlueprintButton->GetSubType());
					_selectedBlueprint._textureId = currentBlueprintButton->GetTextureId();
					_ghostImage->AddGhostImage(_selectedBlueprint, coord);
					hitButtons = true;
					break;
				}
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
					_objectPlaced = true;
					_ghostImage->RemoveGhostImage();
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
			_ghostImage->RemoveGhostImage();
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

void TutorialLogic::InputButtons()
{
	System::MouseCoord coord = _controls->GetMouseCoord();
	_ghostImage->Update(coord);

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
			if (_uiTree->IsButtonColliding(currentButton, coord._pos.x, coord._pos.y))
			{
				GUI::BlueprintNode* currentBlueprintButton = static_cast<GUI::BlueprintNode*>(currentButton);
				_selectedBlueprint._blueprint = _objectHandler->GetBlueprintByType(currentBlueprintButton->GetType(), currentBlueprintButton->GetSubType());
				_selectedBlueprint._textureId = currentBlueprintButton->GetTextureId();
				_ghostImage->AddGhostImage(_selectedBlueprint, coord);
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
					_objectPlaced = true;
					_ghostImage->RemoveGhostImage();
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
			_ghostImage->RemoveGhostImage();
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

bool TutorialLogic::IsTutorialCompleted() const
{
	return _tutorialCompleted;
}