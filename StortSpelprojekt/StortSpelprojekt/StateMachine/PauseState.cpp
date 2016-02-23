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
	System::MouseCoord coord = _controls->GetMouseCoord();
	XMFLOAT4 color(0.3f, 0.3f, 0.3f, 1.0f);
	HandleHoverColorOffset("resume", "resume", coord, color);
	HandleHoverColorOffset("mainmenu", "mainmenu", coord, color);
	HandleHoverColorOffset("quit", "quit", coord, color);


	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		if (_uiTree.IsButtonColliding("resume", coord._pos.x, coord._pos.y))
		{
			ChangeState(GetOldState());
			_tutorialState = TutorialState::OLDTUTORIAL;
		}

		if (_uiTree.IsButtonColliding("mainmenu", coord._pos.x, coord._pos.y) || _controls->IsFunctionKeyDown("MENU:MENU"))
		{
			ChangeState(State::MENUSTATE);
			_tutorialState = TutorialState::NEWTUTORIAL;
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
