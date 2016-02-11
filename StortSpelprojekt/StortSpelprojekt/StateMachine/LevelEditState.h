#pragma once

#include "BaseState.h"
#include "BaseEdit.h"

class LevelEditState : public BaseState
{
private:
	BaseEdit* _baseEdit;

	struct LevelHeader
	{
		int _version;
		int _levelSizeX;
		int _levelSizeY;
		int _nrOfGameObjects;
	};

	struct MapData
	{
		int _posX;
		int _posZ;
		float _rotY;
		int _tileType;
	};
	
	bool _trapButtonClick = false;
	bool _unitButtonClick = false;
	bool _decButtonClick = false;

	bool _floorChosen = false;
	bool _wallChosen = false;

	// Temp, should be replaced with blueprint
	struct ToPlace
	{
		ToPlace()
		{
			_type = TRAP;
			_name = "trap_proto";
		}
		Type _type;
		std::string _name;
	} _toPlace;
	

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