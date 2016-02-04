#pragma once
#include "States.h"
#include "../Controls.h"
#include "../ObjectHandler.h"
#include "../../System/Camera.h"
#include "../PickingDevice.h"
#include "../UITree.h"

class BaseState
{
private:

	static State BaseState::_newStateRequest;

protected:

	System::Controls*		_controls;
	ObjectHandler*			_objectHandler;
	GUI::UITree				_uiTree;
	System::Camera*			_camera;
	PickingDevice*			_pickingDevice;
	
	void ChangeState(State newState);

public:
	BaseState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, const std::string& statename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings);
	virtual ~BaseState();

	virtual void Update(float deltaTime) = 0;
	virtual void OnStateEnter() = 0;
	virtual void OnStateExit() = 0;

	State GetNewStateRequest() const;
	GUI::UITree* GetUITree();
	void Resize(System::Settings* settings);
};
