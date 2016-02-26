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
	bool _isLocked[7];
	XMFLOAT4 _OrginLockColor[7];
	int _leaveCheck;
	XMFLOAT2 _buttonPositions[12];
	GUI::Node* _currentList = nullptr;
	int _currentPage = 0;
	std::vector<GUI::Node*>* _objectTabs;
	DirectX::XMFLOAT3* _ambientLight;
	std::vector<GUI::Node*>* _settingsTabs;

	std::map<System::Type, std::string> _typeLists =
	{
		{ System::FLOOR, "floorlist" },
		{ System::WALL, "walllist" },
		{ System::LOOT, "objectivelist" },
		{ System::SPAWN, "entrylist" },
		{ System::TRAP, "traplist" },
		{ System::CAMERA, "traplist" },
		{ System::GUARD, "unitlist" },
		{ System::ENEMY, "unitlist" },
		{ System::FURNITURE, "decorationlist" }
	};

	GUI::TextBox* _currentlySelectedTextBox;
	std::vector<GUI::TextBox> _textBoxes;

	bool _floorChosen = false;
	bool _wallChosen = false;

	SpecificBlueprint _toPlace;

	void ParseGeneralGameObjectData(std::vector<int>* formattedGameObject, GameObject* gameObject);
	void ParseFloorGameObjectData(std::vector<int>* formattedGameObject, Architecture* floor);

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