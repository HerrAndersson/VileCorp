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

bool TutorialLogic::Update(float deltaTime, BaseEdit* baseEdit, ToPlace& toPlace)
{
	bool tutorialComplete = false;
	bool sCameraPlaced = false;
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
			_uiTree->GetNode("anvil")->SetHidden(true);
			_uiTree->GetNode("anvilplace")->SetHidden(false);
		}
		break;
	}
	case TutorialLogic::ANVILPLACE:
	{
		//Check mouse pos with button
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree->IsButtonColliding("AnvilTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			// Temp, should be replaced with blueprint
			toPlace._type = Type::TRAP;
			toPlace._name = "trap_proto";

			//If we placed an anvil.
			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
				baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
				_gold -= toPlace._goldCost;
				_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
				toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();

				_currentStage = TESLA;
				_uiTree->GetNode("anvilplace")->SetHidden(true);
				_uiTree->GetNode("tesla")->SetHidden(false);
			}
		}
		break;
	}
	case TutorialLogic::TESLA:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = TESLAPLACE;
			_uiTree->GetNode("teslaplace")->SetHidden(true);
			_uiTree->GetNode("securitycamera")->SetHidden(false);
		}
		break;
	}
	case TutorialLogic::TESLAPLACE:
	{
		//Check mouse pos with button
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree->IsButtonColliding("TeslaTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			// Temp, should be replaced with blueprint
			toPlace._type = Type::TRAP;
			toPlace._name = "tesla_trap";

			//If we placed an anvil.
			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
				baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
				_gold -= toPlace._goldCost;
				_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
				toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();

				_currentStage = SECURITYCAMERA;
				_uiTree->GetNode("teslaplace")->SetHidden(true);
				_uiTree->GetNode("securitycamera")->SetHidden(false);
			}
		}
		break;
	}
	case TutorialLogic::SECURITYCAMERA:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = TESLAPLACE;
			_uiTree->GetNode("securitycamera")->SetHidden(true);
			_uiTree->GetNode("securitycameraplace")->SetHidden(false);
		}
		break;
	}
	case TutorialLogic::SECURITYCAMERAPLACE:
	{
		//Check mouse pos with button
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree->IsButtonColliding("Camera", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			// Temp, should be replaced with blueprint
			toPlace._type = Type::CAMERA;
			toPlace._name = "camera_proto";

			//If we placed an anvil.
			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
				baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
				_gold -= toPlace._goldCost;
				_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
				toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();
				sCameraPlaced = true;
			}
			if (_controls->IsFunctionKeyDown("MENU:CONTINUE") && sCameraPlaced)
			{
				_currentStage = BUDGETEXPLAINED;
				_uiTree->GetNode("securitycameraplace")->SetHidden(true);
				_uiTree->GetNode("budgetexplained")->SetHidden(false);
			}
		}
		break;
	}
	case TutorialLogic::BUDGETEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = TESLAPLACE;
			_uiTree->GetNode("budget")->SetHidden(true);
			_uiTree->GetNode("playexplained")->SetHidden(false);
		}
		break;
	}
	case TutorialLogic::PLAYEXPLAINED:
	{
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree->IsButtonColliding("Play", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			tutorialComplete = true;
		}
		break;
	}
	default:
		break;
	}

	return tutorialComplete;
}

