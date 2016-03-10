#include "OptionsState.h"

#include "../Game.h"

OptionsState::OptionsState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: BaseState (controls, objectHandler, camera, pickingDevice, filename, assetManager, fontWrapper, settingsReader, soundModule)
{
	_showApplyButton = false;

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

	_volume = 10;
	_brightness = 5;

	XMFLOAT4 color(0.1f, 0.1f, 0.1f, 1.0f);
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("res_left"), color));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("win_left"), color));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("aa_left"), color));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("shadow_left"), color));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("sound_left"), color));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("brightness_left"), color));

	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("res_right"), color));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("win_right"), color));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("aa_right"), color));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("shadow_right"), color));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("sound_right"), color));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("brightness_right"), color));
	
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("cancel"), color));
	_buttonHighlights.push_back(GUI::HighlightNode(_uiTree.GetNode("apply"), color));

	_showApplyButton = false;
	_buttonClicked = false;
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
	text->SetText(options[optionValue]._text);
}

bool OptionsState::HandleOptionSwitch(const std::string& leftId, const std::string& rightId, const std::string& contentId, int& optionValue, Options* options, int optionsMin, int optionsMax, bool updateContent)
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
	if (optionValue == optionsMin-1)
	{
		optionValue = optionsMax - 1;
	}
	else if (optionValue == optionsMax)
	{
		optionValue = optionsMin;
	}
	if (updateContent && (leftClicked || rightClicked))
	{
		UpdateText(contentId, optionValue, options);
	}
	_buttonClicked = leftClicked || rightClicked;
	return leftClicked || rightClicked;
}

void OptionsState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/GUI/options.json");
	}
	System::MouseCoord coord = _controls->GetMouseCoord();
	HandleButtonHighlight(coord);

	if (_controls->IsFunctionKeyDown("MENU:MENU"))
	{
		_soundModule->Play("page");
		ChangeState(State::MENUSTATE);
	}
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		_showApplyButton = HandleOptionSwitch("res_left", "res_right", "res_content", _resolutionOption, _resolution, 0, RESOLUTION_MAX) || _showApplyButton;
		_showApplyButton = HandleOptionSwitch("win_left", "win_right", "win_content", _windowOption, _window, 0, WINDOW_MAX) || _showApplyButton;
		_showApplyButton = HandleOptionSwitch("aa_left", "aa_right", "aa_content", _aaOption, _aa, 0, AA_MAX) || _showApplyButton;
		_showApplyButton = HandleOptionSwitch("shadow_left", "shadow_right", "shadow_content", _shadowmapOption, _shadowmap, 0, SHADOWMAP_MAX) || _showApplyButton;

		//Handle volume option
		_showApplyButton = HandleOptionSwitch("sound_left", "sound_right", "sound_content", _volume, nullptr, 0, 11, false) || _showApplyButton;
		GUI::Node* text = _uiTree.GetNode("sound_content");
		text->SetText(std::to_wstring(_volume * 10) + L"%");

		//Handle brightness option
		_showApplyButton = HandleOptionSwitch("brightness_left", "brightness_right", "brightness_content", _brightness, nullptr, 1, 11, false) || _showApplyButton;
		GUI::Node* bText = _uiTree.GetNode("brightness_content");
		bText->SetText(std::to_wstring(_brightness));

		_uiTree.GetNode("apply")->SetHidden(!_showApplyButton);

		//Check it the apply button was pressed and change settings file and update the window resolution if needed
		if (_showApplyButton && _uiTree.IsButtonColliding("apply", coord._pos.x, coord._pos.y))
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

			//Sound volume
			settings->_volume = _volume * 10;
			
			//brightness
			settings->_brightness = _brightness;

			//Window options
			if (_window[_windowOption]._value == 1) //Fullscreen
			{
				settings->_windowWidth = GetSystemMetrics(SM_CXSCREEN);
				settings->_windowHeight = GetSystemMetrics(SM_CYSCREEN);
				settings->_screenMode = System::FULLSCREEN;
			}
			else if (_window[_windowOption]._value == 2) //Borderless window
			{
				settings->_screenMode = System::BORDERLESS;
			}
			else if (_window[_windowOption]._value == 3) //Windowed
			{
				settings->_screenMode = System::WINDOWED;
			}
			settings->_showMouseCursor = true;
			settings->_antialiasing = _aa[_aaOption]._value;

			_settingsReader->ApplySettings();
			_uiTree.GetNode("apply")->SetHidden(true);
			_showApplyButton = false;
		}
		if (_uiTree.IsButtonColliding("cancel", coord._pos.x, coord._pos.y))
		{
			_soundModule->Play("page");
			_uiTree.GetNode("apply")->SetHidden(true);
			_showApplyButton = false;
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
		settings->_screenMode == System::FULLSCREEN)
	{
		_windowOption = 0;
	}
	//Borderless window
	else if(settings->_screenMode == System::BORDERLESS)
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

	_volume = settings->_volume / 10;
	GUI::Node* text = _uiTree.GetNode("sound_content");
	text->SetText(std::to_wstring(_volume * 10) + L"%");

	_brightness = settings->_brightness;
	GUI::Node* bText = _uiTree.GetNode("brightness_content");
	bText->SetText(std::to_wstring(_brightness));

	_showApplyButton = false;
	_buttonClicked = false;
}

void OptionsState::OnStateExit()
{

}