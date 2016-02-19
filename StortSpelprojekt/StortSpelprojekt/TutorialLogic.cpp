#include "TutorialLogic.h"

TutorialLogic::TutorialLogic(GUI::UITree* uiTree, System::Controls* controls)
{
	_uiTree = uiTree;
	_controls = controls;
	_gold = 500;
	ResetUiTree();
}

TutorialLogic::~TutorialLogic()
{
}

void TutorialLogic::ResetUiTree()
{
	_currentStage = WELCOME;
	_uiTree->GetNode("welcome")->SetHidden(false);
	_uiTree->GetNode("controls")->SetHidden(true);
	_uiTree->GetNode("objective")->SetHidden(true);
	_uiTree->GetNode("guard")->SetHidden(true);
	_uiTree->GetNode("guardplace")->SetHidden(true);
	_uiTree->GetNode("anvil")->SetHidden(true);
}

void TutorialLogic::Update(float deltaTime, BaseEdit* baseEdit, ToPlace& toPlace)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree->ReloadTree("../../../../StortSpelprojekt/Assets/GUI/placement.json");
		ResetUiTree();
	}

	switch (_currentStage)
	{
	case TutorialLogic::WELCOME:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = CONTROLS;
			_uiTree->GetNode("welcome")->SetHidden(true);
			_uiTree->GetNode("controls")->SetHidden(false);
		}
		break;
	}
	case TutorialLogic::CONTROLS:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = OBJECTIVE;
			_uiTree->GetNode("controls")->SetHidden(true);
			_uiTree->GetNode("objective")->SetHidden(false);
		}
		break;
	}
	case TutorialLogic::OBJECTIVE:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = GUARD;
			_uiTree->GetNode("objective")->SetHidden(true);
			_uiTree->GetNode("guard")->SetHidden(false);
		}
		break;
	}
	case TutorialLogic::GUARD:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = GUARDPLACE;
			_uiTree->GetNode("guard")->SetHidden(true);
			_uiTree->GetNode("guardplace")->SetHidden(false);
		}
		break;
	}
	case TutorialLogic::GUARDPLACE:
	{
		//Check mouse pos with button
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree->IsButtonColliding("Guard", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			// Temp, should be replaced with blueprint
			toPlace._type = Type::GUARD;
			toPlace._name = "guard_proto";

			//If we placed a guard.
			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
				baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
				_gold -= toPlace._goldCost;
				_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
				toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();

				_currentStage = ANVIL;
				_uiTree->GetNode("guardplace")->SetHidden(true);
				_uiTree->GetNode("anvil")->SetHidden(false);
			}
		}
		break;
	}
	case TutorialLogic::ANVIL:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = ANVIL;
			_uiTree->GetNode("guardplace")->SetHidden(true);
			_uiTree->GetNode("anvil")->SetHidden(false);
		}
		break;
	}
	case TutorialLogic::TESLA:
		break;
	case TutorialLogic::SECURITYCAMERA:
		break;
	case TutorialLogic::BUDGET:
		break;
	case TutorialLogic::PLAY:
		break;
	default:
		break;
	}
}

