#pragma once

#include "BaseState.h"
#include "BaseEdit.h"

class LevelEditState : public BaseState
{
private:
	BaseEdit _baseEdit;

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
	
	int _listId;
	XMFLOAT2 _buttonPositions[7];
	GUI::Node* _currentList = nullptr;
	int _currentPage = 0;
	std::vector<GUI::Node*>* objectTabs;

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
	LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	virtual ~LevelEditState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
	
	void HandleInput();
	void HandleButtons();

	void InitNewLevel();
	void ExportLevel();
};