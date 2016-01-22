#pragma once
#include "../Controls.h"
#include "../ObjectHandler.h"
#include "../../System/Camera.h"
#include "../PickingDevice.h"
#include "../UITree.h"

enum State
{
	SPLASHSTATE,
	MENUSTATE,
	PLAYSTATE,
	PLACEMENTSTATE,
	LEVELEDITSTATE,
	OPTIONSSTATE,
	EXITSTATE,
};

class BaseState
{
private:
	static State BaseState::_newStateRequest;

protected:
	System::Controls*		_controls;
	ObjectHandler*			_objectHandler;
	GUI::UITree				_uiTree;
	System::InputDevice*	_inputDevice;
	System::Camera*			_camera;
	PickingDevice*			_pickingDevice;
	
	void ChangeState(State newState);
public:
	BaseState(System::Controls* controls, ObjectHandler* objectHandler,  System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, const std::string& statename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
	virtual ~BaseState();

	virtual void Update(float deltaTime) = 0;
	virtual void OnStateEnter() = 0;
	virtual void OnStateExit() = 0;

	State GetNewStateRequest() const;
	GUI::UITree* GetUITree();
	void Resize(int width, int height);
};
