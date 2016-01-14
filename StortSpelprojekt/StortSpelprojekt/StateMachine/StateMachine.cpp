#include "StateMachine.h"

StateMachine::StateMachine(InitVar initVar)
{
	_currentState = State::SPLASHSTATE;

	_baseStates.push_back(new SplashState(initVar));
	_baseStates.push_back(new MenuState(initVar));
	_baseStates.push_back(new PlayState(initVar));
	_baseStates.push_back(new PlacementState(initVar));
	_baseStates.push_back(new LevelEditState(initVar));
	_baseStates.push_back(new OptionsState(initVar));
}

StateMachine::~StateMachine()
{
	for (auto s : _baseStates)
	{
		delete s;
	}
}

void StateMachine::Update(float deltaTime)
{
	switch (_currentState)
	{
	case EXITSTATE:
	{
		break;
	}
	default:
	{
		_baseStates[_currentState]->Update(deltaTime);
		break;
	}
	}
	ProcessStateRequest();
}

void StateMachine::ProcessStateRequest()
{
	//if currentState does not match with the new requested state.
	if (_currentState != _baseStates[_currentState]->GetNewStateRequest())
	{
		_baseStates[_currentState]->OnStateExit();
		_currentState = _baseStates[_currentState]->GetNewStateRequest();
		_baseStates[_currentState]->OnStateEnter();
	}
}