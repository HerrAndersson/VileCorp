#pragma once

#include "BaseState.h"
#include "SettingsReader.h"

class OptionsState : public BaseState
{
private:
	struct Options
	{
		std::wstring _text;
		int _value;
		int _value2;
	};

	static const int RESOLUTION_MAX = 4;
	Options _resolution[RESOLUTION_MAX];
	int _resolutionOption;

	static const int WINDOW_MAX = 3;
	Options _window[WINDOW_MAX];
	int _windowOption;

	static const int SHADOWMAP_MAX = 4;
	Options _shadowmap[SHADOWMAP_MAX];
	int _shadowmapOption;

	static const int AA_MAX = 2;
	Options _aa[WINDOW_MAX];
	int _aaOption;

	int _volume;
	int _brightness;

	bool _showApplyButton;
	bool _buttonClicked;

	GUI::Node* _applyButtonNode = nullptr;
	GUI::Node* _cancelButtonNode = nullptr;

private:
	int ReadSetting(int setting, int setting2, Options* arr, int max);
	void UpdateText(const std::string& contentId, int optionValue, Options* options);
	void HandleOptionSwitch(const std::string& leftId, const std::string& rightId, const std::string& contentId, int& optionValue, Options* options, int optionsMin, int optionsMax, bool updateContent = true);
	void ApplySettings();

public:
	OptionsState(System::Controls * controls, ObjectHandler * objectHandler, System::Camera * camera, PickingDevice * pickingDevice, const std::string & filename, AssetManager * assetManager, FontWrapper * fontWrapper, System::SettingsReader * settingsReader, System::SoundModule * soundModule);
	virtual ~OptionsState();

	void Update(float deltaTime);

	void OnStateEnter();
	void OnStateExit();

};