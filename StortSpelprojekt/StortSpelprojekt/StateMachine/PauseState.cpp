#include "PauseState.h"

PauseState::PauseState(System::Controls * controls, ObjectHandler * objectHandler, System::Camera * camera, PickingDevice * pickingDevice, const std::string & filename, AssetManager * assetManager, FontWrapper * fontWrapper, System::Settings * settings, System::SoundModule * soundModule)
	: BaseState(controls, objectHandler, camera, pickingDevice, filename, "PAUSE", assetManager, fontWrapper, settings, soundModule)
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
