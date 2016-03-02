#include "PauseState.h"

PauseState::PauseState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("resume")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("mainmenu")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("quit")));
}

PauseState::~PauseState()
{
}

void PauseState::Update(float deltaTime)
{
	System::MouseCoord coord = _controls->GetMouseCoord();
	HandleButtonHighlight(coord);

	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		if (_uiTree.IsButtonColliding("resume", coord._pos.x, coord._pos.y))
		{
			ChangeState(GetOldState());
		}

		if (_uiTree.IsButtonColliding("mainmenu", coord._pos.x, coord._pos.y) || _controls->IsFunctionKeyDown("MENU:MENU"))
		{
			// Tutorial
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
