#pragma once

#include "BaseState.h"
#include "../BaseEdit.h"
#include "LevelFormat.h"

class LevelEditState : public BaseState
{
private:
	BaseEdit* _baseEdit;

	LevelHeader _levelHeader;

	std::string _currentLevelFileName;
	bool _isNewLevel;
	int _listId;
	int _moveCheck;
	bool _pageCheck;
	bool _isPressed[4];
	XMFLOAT2 _buttonPositions[11];
	GUI::Node* _currentList = nullptr;
	int _currentPage = 0;
	std::vector<GUI::Node*>* _objectTabs;

	bool _floorChosen = false;
	bool _wallChosen = false;

	SpecificBlueprint _toPlace;
	
	int GetVectorIndexOfString(std::vector<std::string>* vec ,std::string str);

public:
	LevelEditState(System::Controls * controls, ObjectHandler * objectHandler, System::Camera * camera, PickingDevice * pickingDevice, const std::string & filename, AssetManager * assetManager, FontWrapper * fontWrapper, System::SettingsReader * settingsReader, System::SoundModule * soundModule);
	virtual ~LevelEditState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
	
	void HandleInput();
	void HandleButtons();

	void ExportLevel();
};