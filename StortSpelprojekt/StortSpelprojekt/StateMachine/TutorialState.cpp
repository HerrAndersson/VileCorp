#include "TutorialState.h"

void TutorialState::ChangeTutorialStages(bool forward/* = true*/)
{
	_uiTree.GetNode("Tutorial")->GetChildren()->at(_currentStage)->SetHidden(true);
	_currentStage = TutorialStage(_currentStage + 1);
	_uiTree.GetNode("Tutorial")->GetChildren()->at(_currentStage)->SetHidden(false);
}

void TutorialState::PlacementGuideCheck(std::string nodeName, System::Type type, int subType /*= 0*/)
{
	if (_currentButton == nullptr)
	{
		_currentButton = _uiTree.GetNode(nodeName);
	}

	//Blink button
	if (_light)
	{
		_currentButton->SetColorOffset(DirectX::XMFLOAT4(0.6f, 0.6f, 0.0f, 1.0f));
	}
	else
	{
		_currentButton->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
	}

	//TODO: Add inputhandling to only accept the correct button --Victor / Zache

	if (_baseEdit->IsObjectDropValid() && _baseEdit->GetCreatedObject()->GetType() == type && _baseEdit->GetCreatedObject()->GetSubType() == subType)
	{
		_objectPlaced = true;
	}
	if (_objectPlaced)
	{
		ChangeTutorialStages();
		_objectPlaced = false;
		_currentButton = nullptr;
		_uiTree.GetNode(nodeName)->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		_baseEdit->RemoveGhostImage();
	}
}

TutorialState::TutorialState(System::Controls * controls, ObjectHandler * objectHandler, System::Camera * camera, PickingDevice * pickingDevice, const std::string & filename, AssetManager * assetManager, FontWrapper * fontWrapper, 
							System::SettingsReader * settingsReader, System::SoundModule * soundModule, DirectX::XMFLOAT3 * ambientLight)
	:PlacementState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule, ambientLight)
{
	_sCameraPlaced = false;
	_tutorialCompleted = false;
	_time = 0;
	_light = true;
	_objectPlaced = false;
	_currentButton = nullptr;
	ResetUiTree();
}


TutorialState::~TutorialState()
{
	ResetUiTree();
	_uiTree.GetNode("welcome")->SetHidden(true);
}

void TutorialState::Update(float deltaTime)
{
	_time += deltaTime;
	//icon blink speed
	if (_time > 800)
	{
		_time = 0;
		_light = !_light;
	}
	PlacementState::Update(deltaTime);
	System::MouseCoord coord = _controls->GetMouseCoord();

	switch (_currentStage)
	{
	case WELCOME:
	case CONTROLS:
	case OBJECTIVE:
	case GUARDEXPLAINED:
	case ANVILEXPLAINED:
	case TESLAEXPLAINED:
	case SECURITYCAMERAEXPLAINED:
	case BUDGETEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			ChangeTutorialStages();
		}
	}
		break;
	case GUARDPLACE:
		PlacementGuideCheck("Guard", System::GUARD);
		break;
	case ANVILPLACE:
		PlacementGuideCheck("AnvilTrap", System::TRAP);
		break;
	case TESLAPLACE:
		PlacementGuideCheck("TeslaTrap", System::TRAP, TrapType::TESLACOIL);
		break;
	case SECURITYCAMERAPLACE:
		PlacementGuideCheck("Camera", System::CAMERA);
		break;
	case PLAYEXPLAINED:
		//Blink button
		if (_light)
		{
			_uiTree.GetNode("Play")->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			_uiTree.GetNode("Play")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		
		//TODO: Add inputhandling to only accept the correct button --Victor / Zache

		if (_uiTree.IsButtonColliding("Play", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			_profile->_firstTime = false;
			_tutorialCompleted = true;
			_uiTree.GetNode("Play")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
			ChangeState(PLAYSTATE);
		}
		break;
	default:
		break;
	}
}

void TutorialState::OnStateEnter()
{
	PlacementState::OnStateEnter();
	ResetUiTree();
}

void TutorialState::ResetUiTree()
{
	_tutorialCompleted = false;
	_currentStage = WELCOME;
	_uiTree.GetNode("welcome")->SetHidden(false);
	_uiTree.GetNode("controls")->SetHidden(true);
	_uiTree.GetNode("objective")->SetHidden(true);
	_uiTree.GetNode("guardexplained")->SetHidden(true);
	_uiTree.GetNode("guardplace")->SetHidden(true);
	_uiTree.GetNode("anvilexplained")->SetHidden(true);
	_uiTree.GetNode("anvilplace")->SetHidden(true);
	_uiTree.GetNode("teslaexplained")->SetHidden(true);
	_uiTree.GetNode("teslaplace")->SetHidden(true);
	_uiTree.GetNode("securitycameraexplained")->SetHidden(true);
	_uiTree.GetNode("securitycameraplace")->SetHidden(true);
	_uiTree.GetNode("budgetexplained")->SetHidden(true);
	_uiTree.GetNode("playexplained")->SetHidden(true);
}

bool TutorialState::IsTutorialCompleted() const
{
	return _tutorialCompleted;
}

void TutorialState::HandleButtons()
{
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT") && _currentButton != nullptr)
	{
		bool create = false;
		System::MouseCoord coord = _controls->GetMouseCoord();

		if (_uiTree.IsButtonColliding(_currentButton, coord._pos.x, coord._pos.y))
		{
			GUI::BlueprintNode* currentBlueprintButton = static_cast<GUI::BlueprintNode*>(_currentButton);
			_toPlace._sB._blueprint = _objectHandler->GetBlueprintByType(currentBlueprintButton->GetType(), currentBlueprintButton->GetSubType());
			_toPlace._sB._textureId = currentBlueprintButton->GetTextureId();
			create = true;
		}
		if (create)
		{
			EvaluateGoldCost();
			_baseEdit->HandleBlueprint(&_toPlace._sB);
			_toPlace._blueprintID = _baseEdit->GetCreatedObject()->GetID();
		}
	}
}
