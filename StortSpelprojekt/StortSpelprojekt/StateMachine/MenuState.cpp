#include "MenuState.h"

MenuState::MenuState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState (controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_soundModule->AddSound("Assets/Sounds/page", 1.0f, 1.0f, true, false);
}

MenuState::~MenuState()
{
	
}

void MenuState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/gui.json", "MENU");
	}

	System::MouseCoord coord = _controls->GetMouseCoord();
	HandleHoverColorOffset("playbutton", "play", coord, XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	HandleHoverColorOffset("leveleditbutton", "leveledit", coord, XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	HandleHoverColorOffset("optionsbutton", "options", coord, XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	HandleHoverColorOffset("exitbutton", "exit", coord, XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));

	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		if (_uiTree.IsButtonColliding("playbutton", coord._pos.x, coord._pos.y))
		{
			_soundModule->Stop("Assets/Sounds/theme");
			_soundModule->Play("Assets/Sounds/page");

			ChangeState(State::LEVELSELECTSTATE);
		}
		if (_uiTree.IsButtonColliding("optionsbutton", coord._pos.x, coord._pos.y))
		{
			_soundModule->Play("Assets/Sounds/page");
			ChangeState(State::OPTIONSSTATE);
		}
		if (_uiTree.IsButtonColliding("leveleditbutton", coord._pos.x, coord._pos.y))
		{
			ChangeState(State::LEVELEDITSTATE);
		}
		if (_uiTree.IsButtonColliding("exitbutton", coord._pos.x, coord._pos.y))
		{
			ChangeState(State::EXITSTATE);
		}
	}
	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		ChangeState(State::EXITSTATE);
	}
}

void MenuState::OnStateEnter()
{
	_objectHandler->UnloadLevel();
}

void MenuState::OnStateExit()
{
	
}