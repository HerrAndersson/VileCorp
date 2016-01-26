#pragma once

#include "BaseState.h"
#include "BaseEdit.h"

class PlacementState : public BaseState
{
private:
	BaseEdit _baseEdit;
	int _budget;
	bool _trapChosen = false;

public:
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};

