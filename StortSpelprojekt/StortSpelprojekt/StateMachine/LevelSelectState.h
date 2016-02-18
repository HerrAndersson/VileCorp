#pragma once
#include "BaseState.h"

class LevelSelectState :
	public BaseState
{
private:
	System::Profile* _profile;
	std::vector<std::string> _levelHeaderFilenames;
	int _levelSelection;
	int _levelSelectionMin;
	int _levelSelectionMax;
public:
	LevelSelectState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule);
	~LevelSelectState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};

