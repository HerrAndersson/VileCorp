#include "BaseState.h"

State BaseState::_newStateRequest;

BaseState::BaseState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, const std::string& statename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule)
	: _uiTree(filename, statename, assetManager, fontWrapper, settings)
{
	_controls			= controls;
	_objectHandler		= objectHandler;
	_camera				= camera;
	_pickingDevice		= pickingDevice;
	_newStateRequest	= SPLASHSTATE;
	_soundModule		= soundModule;
}

BaseState::~BaseState()
{}

void BaseState::ChangeState(State newState)
{
	_newStateRequest = newState;
}

State BaseState::GetNewStateRequest()const
{
	return _newStateRequest;
}

GUI::UITree* BaseState::GetUITree()
{
	return &_uiTree;
}

void BaseState::Resize(System::Settings* settings)
{
	_uiTree.Resize(settings);
}