#pragma once

#include "BaseState.h"
#include "BaseEdit.h"

class PlacementState : public BaseState
{
private:
	BaseEdit _baseEdit;
	bool _trapChosen = false;
	int _budget;

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
	PlacementState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	~PlacementState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();

	void HandleInput();
	void HandleButtons();
};

