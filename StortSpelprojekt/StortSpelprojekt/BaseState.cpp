#include "BaseState.h"

State BaseState::_newStateRequest;

BaseState::BaseState(InitVar initVar)
{
	_objectHandler		= initVar._objectHandler;
	_uiHandler			= initVar._uiHandler;
	_inputDevice		= initVar._inputDevice;
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