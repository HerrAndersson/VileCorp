#include "BaseState.h"

State BaseState::_newStateRequest;

BaseState::BaseState(System::Controls* controls, ObjectHandler* objectHandler, System::InputDevice* inputDevice, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, const std::string& statename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
	: _uiTree(filename, statename, assetManager, fontWrapper, width, height)
{
	_controls			= controls;
	_objectHandler		= objectHandler;
	_inputDevice		= inputDevice;
	_camera				= camera;
	_pickingDevice		= pickingDevice;
	_newStateRequest	= SPLASHSTATE;
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

void BaseState::Resize(int width, int height)
{
	_uiTree.Resize(width, height);
}