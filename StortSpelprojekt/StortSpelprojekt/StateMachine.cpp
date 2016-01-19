#include "StateMachine.h"

StateMachine::StateMachine(InitVar initVar, const std::string& filename, AssetManager* assetManager)
{
	_currentState = State::SPLASHSTATE;

	_baseStates.push_back(new SplashState(initVar, filename, assetManager));
	_baseStates.push_back(new MenuState(initVar, filename, assetManager));
	_baseStates.push_back(new PlayState(initVar, filename, assetManager));
	_baseStates.push_back(new OptionsState(initVar, filename, assetManager));
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
	case SPLASHSTATE:
	{
		_baseStates[_currentState]->Update(deltaTime);
		break;
	}
	case MENUSTATE:
	{
		_baseStates[_currentState]->Update(deltaTime);
		break;
	}
	case PLAYSTATE:
	{
		_baseStates[_currentState]->Update(deltaTime);
		break;
	}

	case OPTIONSSTATE:
	{
		_baseStates[_currentState]->Update(deltaTime);
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
	if (_currentState != _baseStates[_currentState]->GetNewStateRequest())
	{
		_baseStates[_currentState]->OnStateExit();
		_currentState = _baseStates[_currentState]->GetNewStateRequest();
		_baseStates[_currentState]->OnStateEnter();
	}
}

BaseState* StateMachine::GetCurrentState() const
{
	return _baseStates[_currentState];
}