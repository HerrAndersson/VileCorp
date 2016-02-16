#include "BaseState.h"

State BaseState::_newStateRequest;
State BaseState::_oldState;

BaseState::BaseState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, const std::string& statename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule)
	: _uiTree(filename, statename, assetManager, fontWrapper, settings)
{
	_controls			= controls;
	_objectHandler		= objectHandler;
	_assetManager		= assetManager;
	_camera				= camera;
	_pickingDevice		= pickingDevice;
	_newStateRequest	= SPLASHSTATE;
	_oldState			= EXITSTATE;
	_soundModule		= soundModule;
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