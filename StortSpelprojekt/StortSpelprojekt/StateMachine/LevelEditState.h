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

	bool _floorChosen = false;
	bool _wallChosen = false;

public:
	LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	virtual ~LevelEditState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
	void HandleInput();
	void InitNewLevel();
	void ExportLevel();
};