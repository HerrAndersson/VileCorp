#include "PlacementState.h"

PlacementState::PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, DirectX::XMFLOAT3* ambientLight)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_buttons = _uiTree.GetNode("UnitList")->GetChildren();
	_profile = settingsReader->GetProfile();

	//Money
	_budget = -1;

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
	int mainType = _toPlace._sB._blueprint->_type;
	int subType = -1;

	if (mainType == System::Type::GUARD)
	{
		subType = _toPlace._sB._blueprint->_subType;
		switch (subType)
		{
		case GuardType::BASICGUARD:
			_toPlace._goldCost = 200;		//GUARD COST
			break;
		case GuardType::ENGINEER:
			_toPlace._goldCost = 100;		//ENGINEER COST
			break;
		}
	}
	else if (mainType == System::Type::CAMERA)
	{
		_toPlace._goldCost = 20;			//CAMERA COST
	}
	else if (mainType == System::Type::TRAP)
	{
		subType = _toPlace._sB._blueprint->_subType;
		switch (_toPlace._sB._blueprint->_subType)
		{
		case TrapType::ANVIL:
			_toPlace._goldCost = 50;		//ANVIL COST
			break;
		case TrapType::BEAR:
			_toPlace._goldCost = 80;		//BEAR COST
			break;
		case TrapType::SAW:
			_toPlace._goldCost = 150;		//SAW COST
			break;
		case TrapType::GUN:
			_toPlace._goldCost = 100;		//MACHIENE GUN COST
			break;
		case TrapType::FLAMETHROWER:
			_toPlace._goldCost = 120;		//FLAMETHROWER COST
			break;
		case TrapType::WATER_GUN:
			_toPlace._goldCost = 80;		//WATER GUN COST
			break;
		case TrapType::TESLACOIL:
			_toPlace._goldCost = 100;		//TESLA COST
			break;
		case TrapType::SPIN_TRAP:
			_toPlace._goldCost = 150;		//SPINTRAP COST
			break;
		case TrapType::CAKEBOMB:
			_toPlace._goldCost = 80;		//SUGARBOMB COST
			break;
		case TrapType::SHARK:
			_toPlace._goldCost = 250;		//SHARK COST
			break;
		}
	}
}

PlacementState::~PlacementState()
{
	delete _baseEdit;
}

void PlacementState::Update(float deltaTime)
{

	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		ChangeState(PAUSESTATE);
	}

	System::MouseCoord coord = _controls->GetMouseCoord();
	
	//Handle the buttons normally
	HandleButtons();
	HandleDescriptions();

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

	//Do only when you don't come from pause state
	if (GetOldState() != State::PAUSESTATE)
	{
		_budget = _objectHandler->GetCurrentLevelHeader()->_budget;

		XMFLOAT3 campos;
		campos.x = (float)_objectHandler->GetTileMap()->GetWidth() / 2.0f;
		campos.y = 15.0f;
		campos.z = (float)_objectHandler->GetTileMap()->GetHeight() / 2.0f - 10.0f;
		_camera->SetPosition(campos);
	}

	_uiTree.GetNode("BudgetValue")->SetText(to_wstring(_budget));

	_buttonHighlights.clear();
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("Play")));

	//Hide all descs
	_uiTree.GetNode("GuardDescription")->SetHidden(true);
	_uiTree.GetNode("EngineerDescription")->SetHidden(true);
	_uiTree.GetNode("CameraDescription")->SetHidden(true);
	_uiTree.GetNode("AnvilDescription")->SetHidden(true);
	_uiTree.GetNode("BearDescription")->SetHidden(true);
	_uiTree.GetNode("SawDescription")->SetHidden(true);
	_uiTree.GetNode("MachineGunDescription")->SetHidden(true);
	_uiTree.GetNode("FlameThrowerDescription")->SetHidden(true);
	_uiTree.GetNode("WaterGunDescription")->SetHidden(true);
	_uiTree.GetNode("TeslaDescription")->SetHidden(true);
	_uiTree.GetNode("SpinDescription")->SetHidden(true);
	_uiTree.GetNode("CakeDescription")->SetHidden(true);
	_uiTree.GetNode("SharkDescription")->SetHidden(true);

	//TODO: Move hardcoded costs and description to logical location /Rikhard
	/*
	Set text for descriptions
	*/
	//GUARD
	_uiTree.GetNode("GuardCost")->SetText(L"Damage: Medium\nHP: " + to_wstring(100) + L"\nSpeed: Medium\nEffect: Average Joe");
	//ENGINEER
	_uiTree.GetNode("EngineerCost")->SetText(L"Damage: None\nHP: " + to_wstring(50) + L"\nSpeed: Fast\nEffect: Fast repair, Passive");
	//CAMERA
	_uiTree.GetNode("CameraCost")->SetText(L"Damage: None\nUses: Infinite\nAtk. Speed: None\nEffect: Vision");
	//ANVIL TRAP
	_uiTree.GetNode("AnvilCost")->SetText(L"Damage: Medium high\nUses: 3\nAtk. Speed: Slow\nEffect: Cheap");
	//BEAR TRAP
	_uiTree.GetNode("BearCost")->SetText(L"Damage: High\nUses: 1\nAtk. Speed: Medium\nEffect: Distraction");
	//SAWBLADE TRAP (FLOOR)
	_uiTree.GetNode("SawCost")->SetText(L"Damage: Super low\nUses: Infinite\nAtk. Speed: Super fast\nEffect: Traversing");
	//MACHINE GUN TRAP
	_uiTree.GetNode("MachineGunCost")->SetText(L"Damage: Medium Low\nUses: 10\nAtk. Speed: Fast\nEffect: None");
	//FLAMETHROWER TRAP
	_uiTree.GetNode("FlameThrowerCost")->SetText(L"Damage: Low\nUses: 5\nAtk. Speed: Fast\nEffect: Burn damage");
	//WATER GUN TRAP
	_uiTree.GetNode("WaterGunCost")->SetText(L"Damage: None\nUses: 10\nAtk. Speed: Medium\nEffect: Slow");
	//TESLA TRAP
	_uiTree.GetNode("TeslaCost")->SetText(L"Damage: Medium low\nUses: 3\nAtk. Speed: Slow\nEffect: Stun");
	//SPIN TRAP
	_uiTree.GetNode("SpinCost")->SetText(L"Damage: None\nUses: Infinite\nAtk. Speed: Very slow\nEffect: Confusion");
	//CAKE TRAP
	_uiTree.GetNode("CakeCost")->SetText(L"Damage: Very high\nUses: 1\nAtk. Speed: Very slow\nEffect: Slow repair");
	//SHARK TRAP
	_uiTree.GetNode("SharkCost")->SetText(L"Damage: Very haj\nUses: Infinite\nAtk. Speed: Very slow\nEffect: Big area");

	std::vector<GUI::Node*>* tutorialNodes = _uiTree.GetNode("Tutorial")->GetChildren();
	for (int i = 0; i < tutorialNodes->size(); i++)
	{
		tutorialNodes->at(i)->SetHidden(true);
	}

	_baseEdit = new BaseEdit(_objectHandler, _controls, _pickingDevice, false);

	//Add icons for noplacement, entries and loot
	_informationOverlayIDs = std::vector<short>();
	AddInformationOverlay();


	//Play music
	_soundModule->Play("in_game_1");

}

void PlacementState::OnStateExit()
{
	delete _baseEdit;
	_baseEdit = nullptr;

	RemoveInformationOverlay();
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
	_uiTree.GetNode("GuardDescription")->SetHidden(!_uiTree.IsButtonColliding("Guard", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("EngineerDescription")->SetHidden(!_uiTree.IsButtonColliding("Engineer", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("CameraDescription")->SetHidden(!_uiTree.IsButtonColliding("Camera", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("AnvilDescription")->SetHidden(!_uiTree.IsButtonColliding("AnvilTrap", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("BearDescription")->SetHidden(!_uiTree.IsButtonColliding("BearTrap", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("SawDescription")->SetHidden(!_uiTree.IsButtonColliding("SawTrap", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("MachineGunDescription")->SetHidden(!_uiTree.IsButtonColliding("MachineGunTrap", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("FlameThrowerDescription")->SetHidden(!_uiTree.IsButtonColliding("FlameThrowerTrap", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("WaterGunDescription")->SetHidden(!_uiTree.IsButtonColliding("WaterGunTrap", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("TeslaDescription")->SetHidden(!_uiTree.IsButtonColliding("TeslaTrap", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("SpinDescription")->SetHidden(!_uiTree.IsButtonColliding("SpinTrap", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("CakeDescription")->SetHidden(!_uiTree.IsButtonColliding("CakeTrap", coord._pos.x, coord._pos.y));
	_uiTree.GetNode("SharkDescription")->SetHidden(!_uiTree.IsButtonColliding("SharkTrap", coord._pos.x, coord._pos.y));
}

void PlacementState::AddInformationOverlay()
{
	for (auto f : *_objectHandler->GetAllByType(System::FLOOR))
	{
		if (static_cast<Architecture*>(f)->GetNoPlacementZone())
		{
			_informationOverlayIDs.push_back(f->GetID());
			XMFLOAT3 pos = f->GetPosition();

			pos.y += 0.0001f;
			ParticleRequestMessage* msg = new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::NOPLACEMENT_SUBTYPE, f->GetID(), pos, XMFLOAT3(0, 1, 0), 1.0f, 1, 0.5f, true, false);
			_objectHandler->GetParticleEventQueue()->Insert(msg);	
		}
	}

	for (auto l : *_objectHandler->GetAllByType(System::LOOT))
	{
		_informationOverlayIDs.push_back(l->GetID());
		XMFLOAT3 pos = l->GetPosition();

		pos.y += 4.0f;
		ParticleRequestMessage* msg = new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::LOOT_SUBTYPE, l->GetID(), pos, XMFLOAT3(0, 1, 0), 1.0f, 1, 0.5f, true, false);
		_objectHandler->GetParticleEventQueue()->Insert(msg);
	}

	for (auto s : *_objectHandler->GetAllByType(System::SPAWN))
	{
		_informationOverlayIDs.push_back(s->GetID());
		XMFLOAT3 pos = s->GetPosition();

		pos.y += 4.0f;
		ParticleRequestMessage* msg = new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::SPAWN_SUBTYPE, s->GetID(), pos, XMFLOAT3(0, 1, 0), 1.0f, 1, 0.5f, true, false);
		_objectHandler->GetParticleEventQueue()->Insert(msg);
	}

}

void PlacementState::RemoveInformationOverlay()
{
	for (auto ID : _informationOverlayIDs)
	{
		_objectHandler->GetParticleEventQueue()->Insert(new ParticleUpdateMessage(ID, false));
	}
}

