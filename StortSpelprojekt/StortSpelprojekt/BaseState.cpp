#include "BaseState.h"

BaseState::BaseState(InitVar initVar)
{
	_objectHandler		= initVar.objectHandler;
	_uiHandler			= initVar.uiHandler;
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