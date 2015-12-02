#include "StateMachine.h"

StateMachine::StateMachine(InitVar initVar)
{
	_currentState = State::SPLASHSTATE;

	for (int i = 0; i < State::EXITSTATE; i++)
	{
		switch (i)
		{
		case SPLASHSTATE:
		{
			_baseState[i] = new SplashState(initVar);
		}
		case MENUSTATE:
		{
			_baseState[i] = new MenuState(initVar);
		}
		case PLAYSTATE:
		{
			_baseState[i] = new PlayState(initVar);
		}
		case OPTIONSSTATE:
		{
			_baseState[i] = new OptionsState(initVar);
		}
		}
	}

}

StateMachine::~StateMachine()
{
	for (int i = 0; i < State::EXITSTATE; i++)
	{
		delete _baseState[i];
	}
}

void StateMachine::Update(float deltaTime)
{
	switch (_currentState)
	{
	case SPLASHSTATE:
	{
		_baseState[_currentState]->Update(deltaTime);
		break;
	}
	case MENUSTATE:
	{
		_baseState[_currentState]->Update(deltaTime);
		break;
	}
	case PLAYSTATE:
	{
		_baseState[_currentState]->Update(deltaTime);
		break;
	}

	case OPTIONSSTATE:
	{
		_baseState[_currentState]->Update(deltaTime);
		break;
	}
	case EXITSTATE:
	{

		break;
	}
	}
	ProcessStateRequest();
}

void StateMachine::ProcessStateRequest()
{
	//if currentState does not match with the new requested state.
	if (_currentState != _baseState[_currentState]->GetNewStateRequest())
	{
		_baseState[_currentState]->OnStateExit();
		_currentState = _baseState[_currentState]->GetNewStateRequest();
		_baseState[_currentState]->OnStateEnter();
	}
}