#include "OptionsState.h"

OptionsState::OptionsState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: BaseState (controls, objectHandler, camera, pickingDevice, filename, "OPTIONS", assetManager, fontWrapper, width, height)
{
	_controls = controls;
	_objectHandler = objectHandler;

	_camera = camera;
	_pickingDevice = pickingDevice;

	//Resolution options
	_resolution[0] = { L"1920x1080", 1920, 1080 };
	_resolution[1] = { L"1600x900", 1600, 900 };
	_resolution[2] = { L"1366x768", 1366, 768 };
	_resolution[3] = { L"1280x720", 1280, 720 };
	_resolutionOption = 0;

	//Window options
	_window[0] = { L"Fullscreen", System::WindowSettings::FULLSCREEN, 0 };
	_window[1] = { L"Borderless window", System::WindowSettings::BORDERLESS, 0 };
	_window[2] = { L"Windowed", 0, 0 };
	_windowOption = 0;

	//Window options
	_shadowmap[0] = { L"256x256", 256, 256 };
	_shadowmap[1] = { L"512x512", 512, 512 };
	_shadowmap[2] = { L"1024x1024", 1024, 1024 };
	_shadowmap[3] = { L"2048x2048", 2048, 2048 };
	_shadowmapOption = 0;
	
	//Antialiasing
	_aa[0] = { L"On", 1, 0 };
	_aa[1] = { L"Off", 0, 0 };
	_aaOption = 0;

	_volumeOption = 100.0f;
}

OptionsState::~OptionsState()
{}

void OptionsState::HandleOptionSwitch(const std::string& leftId, const std::string& rightId, const std::string& contentId, int& optionValue, Options* options, int optionsMax)
{
	System::MouseCoord coord = _controls->GetMouseCoord();
	bool leftClicked = _uiTree.IsButtonColliding(leftId, coord._pos.x, coord._pos.y);
	bool rightClicked = _uiTree.IsButtonColliding(rightId, coord._pos.x, coord._pos.y);
	if (leftClicked)
	{
		optionValue--;
	}
	else if (rightClicked)
	{
		optionValue++;
	}
	if (optionValue == -1)
	{
		optionValue = optionsMax - 1;
	}
	else if (optionValue == optionsMax)
	{
		optionValue = 0;
	}
	if (leftClicked || rightClicked)
	{
		GUI::Node* text = _uiTree.GetNode(contentId);
		if (text)
		{
			text->SetText(options[optionValue]._text);
		}
	}
}

void OptionsState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/gui.json", "OPTIONS");
	}
	if (_controls->IsFunctionKeyDown("MENU:CLICK"))
	{
		HandleOptionSwitch("res_left", "res_right", "res_content", _resolutionOption, _resolution, RESOLUTION_MAX);
		HandleOptionSwitch("win_left", "win_right", "win_content", _windowOption, _window, WINDOW_MAX);
		HandleOptionSwitch("aa_left", "aa_right", "aa_content", _aaOption, _aa, AA_MAX);
		HandleOptionSwitch("shadow_left", "shadow_right", "shadow_content", _shadowmapOption, _shadowmap, SHADOWMAP_MAX);

		//Check it the apply button was pressed and change settings file and update the window resolution if needed
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree.IsButtonColliding("apply", coord._pos.x, coord._pos.y))
		{
			_outputSettings._resX = _resolution[_resolutionOption]._value;
			_outputSettings._resY = _resolution[_resolutionOption]._value2;
			_outputSettings._shadowResX = _shadowmap[_shadowmapOption]._value;
			_outputSettings._shadowResY = _shadowmap[_shadowmapOption]._value2;
			_outputSettings._default = true;
			_outputSettings._shadowmap = true;
			_outputSettings._fullScreen = _window[_windowOption]._value & System::WindowSettings::FULLSCREEN;
			_outputSettings._debugMode = false;
			_outputSettings._bordeless = _window[_windowOption]._value & System::WindowSettings::BORDERLESS;
			_outputSettings._showMouseCursor = _window[_windowOption]._value & System::WindowSettings::SHOW_CURSOR;

			System::saveJSON(&_outputSettings, "../../../../StortSpelprojekt/Assets/GameSettings.json", "Game Settings");
		}
	}
	

	
}

void OptionsState::OnStateEnter()
{

}

void OptionsState::OnStateExit()
{

}