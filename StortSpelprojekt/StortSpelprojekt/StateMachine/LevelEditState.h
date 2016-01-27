#pragma once

#include "BaseState.h"
#include "LevelEdit.h"

class LevelEditState : public BaseState
{
private:
	LevelEdit _levelEdit;

	bool _trapButtonClick = false;
	bool _unitButtonClick = false;
	bool _decButtonClick = false;

	bool _floorChosen = false;
	bool _wallChosen = false;

public:
	LevelEditState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	virtual ~LevelEditState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};