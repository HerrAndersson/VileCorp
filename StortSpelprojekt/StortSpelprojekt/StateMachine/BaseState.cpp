#include "BaseState.h"

State BaseState::_newStateRequest;

BaseState::BaseState(InitVar initVar)
{
	_controls = initVar._controls;
	_objectHandler		= initVar._objectHandler;
	_uiHandler			= initVar._uiHandler;
	_camera				= initVar._camera;
	_pickingDevice		= initVar._pickingDevice;
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