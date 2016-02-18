#include "OptionsState.h"

#include "../Game.h"

OptionsState::OptionsState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState (controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_settingsReader = settingsReader;

	//Resolution options
	_resolution[0] = { L"1280x720", 1280, 720 };
	_resolution[1] = { L"1366x768", 1366, 768 };
	_resolution[2] = { L"1600x900", 1600, 900 };
	_resolution[3] = { L"1920x1080", 1920, 1080 };
	_resolutionOption = 0;

	//Window options
	_window[0] = { L"Fullscreen", 1, 0 };
	_window[1] = { L"Borderless window", 2, 0 };
	_window[2] = { L"Windowed", 3, 0 };
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

	//TODO: Volume setting //Mattias
	_volumeOption = 100.0f;
}

OptionsState::~OptionsState()
{

}

int OptionsState::ReadSetting(int setting, int setting2, Options* options, int max)
{
	int result = 0;
	for (int i = 0; i < max; i++)
	{
		if (setting == options[i]._value && setting2 == options[i]._value2)
		{
			result = i;
			break;
		}
	}
	return result;
}

void OptionsState::UpdateText(const std::string& contentId, int optionValue, Options* options)
{
	GUI::Node* text = _uiTree.GetNode(contentId);
	if (text)
	{
		text->SetText(options[optionValue]._text);
	}
}

bool OptionsState::HandleOptionSwitch(const std::string& leftId, const std::string& rightId, const std::string& contentId, int& optionValue, Options* options, int optionsMax)
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
		UpdateText(contentId, optionValue, options);
	}
	return leftClicked || rightClicked;
}

void OptionsState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/gui.json", "OPTIONS");
	}
	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		_soundModule->Play("Assets/Sounds/page");
		ChangeState(State::MENUSTATE);
	}
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		bool showApplyButton = false;
		showApplyButton = showApplyButton || HandleOptionSwitch("res_left", "res_right", "res_content", _resolutionOption, _resolution, RESOLUTION_MAX);
		showApplyButton = showApplyButton || HandleOptionSwitch("win_left", "win_right", "win_content", _windowOption, _window, WINDOW_MAX);
		showApplyButton = showApplyButton || HandleOptionSwitch("aa_left", "aa_right", "aa_content", _aaOption, _aa, AA_MAX);
		showApplyButton = showApplyButton || HandleOptionSwitch("shadow_left", "shadow_right", "shadow_content", _shadowmapOption, _shadowmap, SHADOWMAP_MAX);

		if (showApplyButton)
		{
			_uiTree.GetNode("apply")->SetHidden(false);
		}

		//Check it the apply button was pressed and change settings file and update the window resolution if needed
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree.IsButtonColliding("apply", coord._pos.x, coord._pos.y))
		{
			System::Settings* settings = _settingsReader->GetSettings();

			//Resolution
			settings->_screenWidth = _resolution[_resolutionOption]._value;
			settings->_screenHeight = _resolution[_resolutionOption]._value2;

			//Window size
			settings->_windowWidth = settings->_screenWidth;
			settings->_windowHeight = settings->_screenHeight;

			//Shadow map resolution
			settings->_shadowMapSize = _shadowmap[_shadowmapOption]._value;
			//Window options
			if (_window[_windowOption]._value == 1) //Fullscreen
			{
				settings->_windowWidth = GetSystemMetrics(SM_CXSCREEN);
				settings->_windowHeight = GetSystemMetrics(SM_CYSCREEN);
				settings->_borderless = true;
			}
			else if (_window[_windowOption]._value == 2) //Borderless window
			{
				settings->_borderless = true;
			}
			else if (_window[_windowOption]._value == 3) //Windowed
			{
				settings->_borderless = false;
			}
			settings->_showMouseCursor = true;
			settings->_antialiasing = _aa[_aaOption]._value;

			_settingsReader->ApplySettings();
			_uiTree.GetNode("apply")->SetHidden(true);
		}
		if (_uiTree.IsButtonColliding("cancel", coord._pos.x, coord._pos.y))
		{
			_soundModule->Play("Assets/Sounds/page");
			_uiTree.GetNode("apply")->SetHidden(true);
			ChangeState(State::MENUSTATE);
		}
	}
}

void OptionsState::OnStateEnter()
{
	System::Settings* settings = _settingsReader->GetSettings();
	unsigned int systemSizeX = GetSystemMetrics(SM_CXSCREEN);
	unsigned int systemSizeY = GetSystemMetrics(SM_CYSCREEN);

	_resolutionOption = ReadSetting(settings->_screenWidth, settings->_screenHeight, _resolution, RESOLUTION_MAX);
	UpdateText("res_content", _resolutionOption, _resolution);

	//Fullscreen
	if (_resolution[_resolutionOption]._value == systemSizeX &&
		_resolution[_resolutionOption]._value2 == systemSizeY &&
		settings->_windowWidth == systemSizeX &&
		settings->_windowHeight == systemSizeY &&
		settings->_borderless)
	{
		_windowOption = 0;
	}
	//Borderless window
	else if(settings->_borderless)
	{
		_windowOption = 1;
	}
	//Windowed
	else
	{
		_windowOption = 2;
	}
	UpdateText("win_content", _windowOption, _window);

	_shadowmapOption = ReadSetting(settings->_shadowMapSize, settings->_shadowMapSize, _shadowmap, SHADOWMAP_MAX);
	UpdateText("shadow_content", _shadowmapOption, _shadowmap);

	_aaOption = ReadSetting(settings->_antialiasing, 0, _aa, AA_MAX);
	UpdateText("aa_content", _aaOption, _aa);
}

void OptionsState::OnStateExit()
{

}