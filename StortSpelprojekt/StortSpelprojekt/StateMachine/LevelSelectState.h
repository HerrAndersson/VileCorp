#pragma once
#include "BaseState.h"
#include "JsonParser.h"
#include "../CombinedMeshGenerator.h"

#define TUTORIAL 0

class LevelSelectState :
	public BaseState
{
private:
	System::Profile* _profile;
	bool _isCampaignMode;
	std::vector<std::string> _skirmishHeaderFilenames;
	Level::LevelHeader _selectedLevelHeader;
	
	GUI::Node* _levelNameNode = nullptr;
	GUI::Node* _storyTitleNode = nullptr;
	GUI::Node* _storyBodyNode = nullptr;
	GUI::Node* _gameModeNode = nullptr;

	GUI::Node* _nextLevelButtonNode = nullptr;
	GUI::Node* _previousLevelButtonNode = nullptr;

	GUI::Node* _campaignTabNode = nullptr;
	GUI::Node* _skirmishTabNode = nullptr;

	XMFLOAT2 _tabPosition[2];

	int _campaignSelection;
	int _campaignSelectionMin;
	int _campaignSelectionMax;

	int _skirmishSelectedIndex;

	void LoadLevelHeader(int levelFilename, Level::LevelHeader* headerToLoad);
	void LoadLevelHeader(std::string levelFilename, Level::LevelHeader* headerToLoad);
	void SelectedLevelHeaderToGUI();
	void UpdateButtonsNextPreviousVisability();
	void SwitchLevelSelectionMode();

	CombinedMeshGenerator* _combinedMeshGenerator;
	
public:

	LevelSelectState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule, CombinedMeshGenerator* combinedMeshGenerator);
	~LevelSelectState();

	void Update(float deltaTime);

	void OnStateEnter();
	void OnStateExit();
};