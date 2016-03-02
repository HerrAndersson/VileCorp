#include "TutorialState.h"

void TutorialState::ChangeTutorialStages(bool forward/* = true*/)
{
	_uiTree.GetNode("Tutorial")->GetChildren()->at(_currentStage)->SetHidden(false);
	_currentStage = TutorialStage(_currentStage + 1);
	_uiTree.GetNode("Tutorial")->GetChildren()->at(_currentStage)->SetHidden(true);
}

void TutorialState::PlacementGuideCheck(std::string nodeName, System::Type type, int subType /*= 0*/)
{
	//Blink button
	if (_light)
	{
		_uiTree.GetNode(nodeName)->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	}
	else
	{
		_uiTree.GetNode(nodeName)->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
	}

	//TODO: Add inputhandling to only accept the correct button --Victor / Zache

	//if (_objectPlaced)
	//{
		ChangeTutorialStages();
		_objectPlaced = false;
		_uiTree.GetNode(nodeName)->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
	//}


}

TutorialState::TutorialState(System::Controls * controls, ObjectHandler * objectHandler, System::Camera * camera, PickingDevice * pickingDevice, const std::string & filename, AssetManager * assetManager, FontWrapper * fontWrapper, 
							System::SettingsReader * settingsReader, System::SoundModule * soundModule, DirectX::XMFLOAT3 * ambientLight)
	:PlacementState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule, ambientLight)
{
	_sCameraPlaced = false;
	_tutorialCompleted = false;
	_time = 0;
	_light = false;
	_objectPlaced = false;
	ResetUiTree();
}


TutorialState::~TutorialState()
{
	ResetUiTree();
	_uiTree.GetNode("welcome")->SetHidden(true);
}

void TutorialState::Update(float deltaTime)
{
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
