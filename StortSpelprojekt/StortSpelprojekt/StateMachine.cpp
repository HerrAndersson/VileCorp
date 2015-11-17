#include "StateMachine.h"

StateMachine::StateMachine()
{
	
}

StateMachine::~StateMachine()
{

}

void StateMachine::Initialize()
{
	_currentState = State::SPLASHSTATE; // State::PLAYSTATE;
}

