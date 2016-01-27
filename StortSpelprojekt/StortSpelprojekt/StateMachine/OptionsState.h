#pragma once

#include "BaseState.h"
#include "JsonParser.h"

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

	float _volumeOption;


	SettingInfo _outputSettings;
private:
	bool HandleOptionSwitch(const std::string& leftId, const std::string& rightId, const std::string& contentId, int& optionValue, Options* options, int optionsMax);

public:
	OptionsState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	virtual ~OptionsState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();

};