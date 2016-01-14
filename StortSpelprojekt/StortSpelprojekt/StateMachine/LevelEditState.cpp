#include "LevelEditState.h"

LevelEditState::LevelEditState(InitVar initVar) : BaseState(initVar)
{
	_initVar = initVar;
}

LevelEditState::~LevelEditState()
{}

void LevelEditState::Update(float deltaTime)
{
	_levelEdit.Update();

	//tempAddObj

	//R Adds Floor
	if (_inputHandler->IsPressed(0x52))
	{
		_levelEdit.Add(FLOOR, FLOOR);
	}

	//T adds Wall
	if (_inputHandler->IsPressed(0x54))
	{
		_levelEdit.Add(WALL, WALL);
	}

	//Y adds Unit
	if (_inputHandler->IsPressed(0x59))
	{
		_levelEdit.Add(UNIT, UNIT);
	}
}

void LevelEditState::OnStateEnter()
{
	_levelEdit.Initialize(&_initVar);
}

void LevelEditState::OnStateExit()
{

}

