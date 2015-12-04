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
			_baseState.push_back(new SplashState(initVar));
			break;
		}
		case MENUSTATE:
		{
			_baseState.push_back(new MenuState(initVar));
			break;
		}
		case PLAYSTATE:
		{
			_baseState.push_back(new PlayState(initVar));
			break;
		}
		case OPTIONSSTATE:
		{
			_baseState.push_back(new OptionsState(initVar));
			break;
		}
		}
	}

}

StateMachine::~StateMachine()
{
	for (auto s : _baseState)
	{
		delete s;
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