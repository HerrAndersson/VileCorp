#include "BaseState.h"

State BaseState::_newStateRequest;
State BaseState::_oldState;
TutorialState BaseState::_tutorialState;
bool BaseState::_tutorial;
bool BaseState::_pausedTutorial;

BaseState::BaseState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: _uiTree(filename, assetManager, fontWrapper, settingsReader)
{
	_controls			= controls;
	_objectHandler		= objectHandler;
	_camera				= camera;
	_pickingDevice		= pickingDevice;
	_newStateRequest	= SPLASHSTATE;
	_oldState			= EXITSTATE;
	_tutorial			= false;
	_pausedTutorial		= false;
	_soundModule		= soundModule;
	_settingsReader		= settingsReader;
	_assetManager		= assetManager;
}

BaseState::~BaseState()
{}

void BaseState::ChangeState(State newState)
{
	_oldState = _newStateRequest;
	_newStateRequest = newState;
	ResetMouse();
}

State BaseState::GetNewStateRequest()const
{
	return _newStateRequest;
}

State BaseState::GetOldState()const
{
	return _oldState;
}

GUI::UITree* BaseState::GetUITree()
{
	return &_uiTree;
}

void BaseState::Resize(System::Settings* settings)
{
	_uiTree.Resize(settings);
}

void BaseState::ResetMouse()
{
	if (_camera->GetMode() != System::LOCKED_CAM)
	{
		_controls->ToggleCursorLock();
		_camera->SetMode(System::LOCKED_CAM);
		_camera->SetRotation(DirectX::XMFLOAT3(70, 0, 0));
	}
}