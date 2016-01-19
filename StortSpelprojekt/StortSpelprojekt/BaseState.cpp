#include "BaseState.h"

State BaseState::_newStateRequest;

BaseState::BaseState(InitVar initVar, const std::string& filename, const std::string& statename) : _uiTree(filename, statename)
{
	_objectHandler		= initVar._objectHandler;
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