#pragma once

#include "BaseState.h"
#include "../BaseEdit.h"
#include "LevelFormat.h"

class LevelEditState : public BaseState
{
private:
	BaseEdit* _baseEdit;

	Level::LevelHeader _levelHeader;

	std::string _currentLevelFileName;
	bool _isNewLevel;
	int _listId;
	int _settingsId;
	int _moveCheck;
	bool _pageCheck;
	bool _isPressed[5];
	bool _isLocked[11];
	int _mapChangeSelected;
	XMFLOAT4 _OrginLockColor[11];
	int _leaveCheck;
	XMFLOAT2 _buttonPositions[12];
	GUI::Node* _currentList = nullptr;
	int _currentPage = 0;
	std::vector<GUI::Node*>* _objectTabs;
	DirectX::XMFLOAT3* _ambientLight;
	std::vector<GUI::Node*>* _settingsTabs;

	std::map<Type, std::string> _typeLists =
	{
		{ FLOOR, "floorlist" },
		{ WALL, "walllist" },
		{ LOOT, "objectivelist" },
		{ SPAWN, "entrylist" },
		{ TRAP, "traplist" },
		{ CAMERA, "traplist" },
		{ GUARD, "unitlist" },
		{ ENEMY, "unitlist" },
		{ FURNITURE, "decorationlist" }
	};

	GUI::TextBox* _currentlySelectedTextBox;
	std::vector<GUI::TextBox> _textBoxes;
	std::vector<std::string> _levelHeaderFilenames;

	bool _floorChosen = false;
	bool _wallChosen = false;

	SpecificBlueprint _toPlace;

public:
	LevelEditState(System::Controls * controls, ObjectHandler * objectHandler, System::Camera * camera, PickingDevice * pickingDevice, const std::string & filename, AssetManager * assetManager, FontWrapper * fontWrapper, System::SettingsReader * settingsReader, System::SoundModule * soundModule, DirectX::XMFLOAT3* ambientLight);
	virtual ~LevelEditState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
	
	void HandleInput();
	bool HandleButtons(); //returns true if any button was clicked

	void ExportLevel();
};