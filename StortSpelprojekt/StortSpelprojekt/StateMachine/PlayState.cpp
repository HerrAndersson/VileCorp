#include "PlayState.h"

PlayState::PlayState(InitVar initVar) : BaseState(initVar)
{
	_initVar = initVar;
}

PlayState::~PlayState()
{}

void PlayState::Update(float deltaTime)
{
	_gameLogic.Update(deltaTime);
}

void PlayState::OnStateEnter()
{
	_gameLogic.Initialize(&_initVar);
}

void PlayState::OnStateExit()
{

}