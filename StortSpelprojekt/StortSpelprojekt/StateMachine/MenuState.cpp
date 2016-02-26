#include "MenuState.h"

MenuState::MenuState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState (controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_soundModule->AddSound("theme", 0.15f, 1.0f, true, true);
	_soundModule->AddSound("page", 1.0f, 1.0f, true, false);

	XMFLOAT4 color(0.1f, 0.1f, 0.1f, 1.0f);
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("playbutton"), color, _uiTree.GetNode("play")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("leveleditbutton"), color, _uiTree.GetNode("leveledit")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("optionsbutton"), color, _uiTree.GetNode("options")));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("exitbutton"), color, _uiTree.GetNode("exit")));
}

MenuState::~MenuState()
{
	
}

void MenuState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/GUI/menu.json");
	}

	System::MouseCoord coord = _controls->GetMouseCoord();

	HandleButtonHighlight(coord);

	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		if (_uiTree.IsButtonColliding("playbutton", coord._pos.x, coord._pos.y))
		{
			_soundModule->Stop("theme");
			_soundModule->Play("page");

			ChangeState(State::LEVELSELECTSTATE);
		}
		if (_uiTree.IsButtonColliding("optionsbutton", coord._pos.x, coord._pos.y))
		{
			_soundModule->Play("page");
			ChangeState(State::OPTIONSSTATE);
		}
		if (_uiTree.IsButtonColliding("leveleditbutton", coord._pos.x, coord._pos.y))
		{
			_soundModule->Stop("theme");
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

	//Restart all music
	_soundModule->Stop("theme");
	_soundModule->Stop("in_game_1");
	_soundModule->Stop("in_game_2");

	//Play theme
	_soundModule->Play("theme");
}

void MenuState::OnStateExit()
{
	
}