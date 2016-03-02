#pragma once

#include "../../System/SoundModule.h"
#include "States.h"
#include "../Controls.h"
#include "../ObjectHandler.h"
#include "../../System/Camera.h"
#include "../PickingDevice.h"
#include "../UITree.h"
#include "../../System/SettingsReader.h"
#include "../TutorialLogic.h"
#include "GUI elements/HighlightNode.h"


class BaseState
{
private:
	static State _newStateRequest;
	static State _oldState; //To make us able to return from pause state back to correct state.

protected:
	static TutorialState	_tutorialState;
	System::Controls*		_controls;
	ObjectHandler*			_objectHandler;
	AssetManager*			_assetManager;
	GUI::UITree				_uiTree;
	System::Camera*			_camera;
	PickingDevice*			_pickingDevice;
	System::SoundModule*	_soundModule;
	System::SettingsReader* _settingsReader;

	std::vector<GUI::HighlightNode> _buttonHighlights;
	
	void ChangeState(State newState);
	void ResetMouse();
	State GetOldState() const;

	void HandleCam(float deltaTime);
	void HandleCamMode();
	void HandleCamZoom();
	void HandleCamRot();
	void HandleCamMove(float deltaTime);

	void HandleButtonHighlight(const System::MouseCoord& coord);
public:
	BaseState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule);
	virtual ~BaseState();

	virtual void Update(float deltaTime) = 0;
	virtual void OnStateEnter() = 0;
	virtual void OnStateExit() = 0;

	State GetNewStateRequest() const;
	GUI::UITree* GetUITree();
	void Resize(System::Settings* settings);
};
