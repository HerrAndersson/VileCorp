#include "StateMachine.h"

StateMachine::StateMachine(InitVar initVar, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height)
{
	_currentState = State::SPLASHSTATE;

	_baseStates.push_back(new SplashState(initVar, filename, assetManager, fontWrapper, width, height));
	_baseStates.push_back(new MenuState(initVar, filename, assetManager, fontWrapper, width, height));
	_baseStates.push_back(new PlayState(initVar, filename, assetManager, fontWrapper, width, height));
	_baseStates.push_back(new OptionsState(initVar, filename, assetManager, fontWrapper, width, height));
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
	_baseStates[_currentState]->Update(deltaTime);
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

void StateMachine::Resize(int width, int height)
{
	for (auto i: _baseStates)
	{
		i->Resize(width, height);
	}
}
