#include "PauseState.h"

PauseState::PauseState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
}

PauseState::~PauseState()
{
}

void PauseState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree.IsButtonColliding("resume", coord._pos.x, coord._pos.y))
		{
			ChangeState(GetOldState());
		}

		if (_uiTree.IsButtonColliding("mainmenu", coord._pos.x, coord._pos.y) || _controls->IsFunctionKeyDown("MENU:MENU"))
		{
			ChangeState(State::MENUSTATE);
			_tutorialStage = TutorialStage::NEWTUTORIAL;
		}

		if (_uiTree.IsButtonColliding("quit", coord._pos.x, coord._pos.y))
		{
			ChangeState(State::EXITSTATE);
		}
	}
}

void PauseState::OnStateEnter()
{
}

void PauseState::OnStateExit()
{
}
