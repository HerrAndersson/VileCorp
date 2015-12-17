#include "LevelEditState.h"

LevelEditState::LevelEditState(InitVar initVar) : BaseState(initVar)
{
	_initVar = initVar;
}

LevelEditState::~LevelEditState()
{}

void LevelEditState::Update(float deltaTime)
{
	_levelEdit.Update(deltaTime);
}

void LevelEditState::OnStateEnter()
{
	_levelEdit.Initialize(&_initVar);
}

void LevelEditState::OnStateExit()
{

}

