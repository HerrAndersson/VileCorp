#include "PlacementState.h"

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_player = new Player(_objectHandler, pickingDevice);
	_buttons = _uiTree.GetNode("UnitList")->GetChildren();
	_profile = settingsReader->GetProfile();
	//_tutorialLogic = new TutorialLogic(&_uiTree, _controls, _player, _buttons, &_ghostImage, objectHandler, pickingDevice, _profile);

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
	_baseEdit = nullptr;

	//Add sound
	_soundModule->AddSound("in_game_1", 0.2f, 1.0f, true, true, true);
}

void PlacementState::EvaluateGoldCost()
{
}

PlacementState::~PlacementState()
{
	delete _baseEdit;
	delete _player;

	//delete _tutorialLogic;
	//_tutorialLogic = nullptr;
}

void PlacementState::Update(float deltaTime)
{

	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		ChangeState(PAUSESTATE);
	}

	System::MouseCoord coord = _controls->GetMouseCoord();
	//if tutorial mode. Then bypass normal baseEdit update loops.
	if (_tutorialState != TutorialState::NOTUTORIAL)
	{
		//bypass the normal UI interface to interface the tutorial elements into it.
		//_tutorialLogic->Update(deltaTime, _baseEdit, _toPlace, _profile);
		//if (_tutorialLogic->IsTutorialCompleted())
		//{
		//	ChangeState(State::PLAYSTATE);
		//	_tutorialState = TutorialState::NOTUTORIAL;
		//}

		//Menu - We keep this outside of tutorial due to the changestate function.
		if (_controls->IsFunctionKeyDown("MENU:MENU"))
		{
			//if (_ghostImage.IsGhostImageActive() || _player->IsSelectedObjects())
			//{
			//	_ghostImage.RemoveGhostImage();
			//	_player->DeselectObjects();
			//}
			//else
			//{
			//	ChangeState(PAUSESTATE);
			//}
		}
	}
	else
	{
		//Handle the buttons normally
		HandleButtons();
		HandleDescriptions();
	}
	HandleButtonHighlight(coord);
	HandleInput();
	HandleCam(deltaTime);

	//baseEdit update handles basic controls.
	_baseEdit->Update();
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
		//_tutorialLogic->ResetUiTree();
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

	_baseEdit = new BaseEdit(_objectHandler, _controls, _pickingDevice, false);

	//Play music
	_soundModule->Play("in_game_1");
}

void PlacementState::OnStateExit()
{
	delete _baseEdit;

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
	if (_baseEdit->IsObjectDropValid())
	{
		if (_budget >= _toPlace._goldCost && _baseEdit->GetCreatedObject() != nullptr)
		{
			_budget -= _toPlace._goldCost;
			_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_budget));
		}
		else
		{
			_objectHandler->Remove(_baseEdit->GetCreatedObject());
		}
	}

	if (_baseEdit->GetDeletedObjectBlueprint() != nullptr)
	{
		_toPlace._sB._blueprint = _baseEdit->GetDeletedObjectBlueprint();
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

	////Menu
	//if (_controls->IsFunctionKeyDown("MENU:MENU"))
	//{
	//	if (_ghostImage.IsGhostImageActive() || _player->IsSelectedObjects())
	//	{
	//		_ghostImage.RemoveGhostImage();
	//		_player->DeselectObjects();
	//	}
	//	else
	//	{
	//		ChangeState(PAUSESTATE);
	//	}
	//}

	//Play
	if (_uiTree.IsButtonColliding("Play", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		ChangeState(PLAYSTATE);
	}

	if(_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		std::vector<GUI::Node*>* units = _uiTree.GetNode("UnitList")->GetChildren();
		for (unsigned y = 0; y < units->size(); y++)
		{
			GUI::Node* currentButton = units->at(y);
			if (_uiTree.IsButtonColliding(currentButton, coord._pos.x, coord._pos.y))
			{
				GUI::BlueprintNode* currentBlueprintButton = static_cast<GUI::BlueprintNode*>(currentButton);
				_toPlace._sB._blueprint = _objectHandler->GetBlueprintByType(currentBlueprintButton->GetType(), currentBlueprintButton->GetSubType());
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
		_toPlace._blueprintID = _baseEdit->GetCreatedObject()->GetID();
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