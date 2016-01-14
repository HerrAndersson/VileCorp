#include "PlacementState.h"



PlacementState::PlacementState(InitVar initVar) : BaseState(initVar)
{
	_initVar = initVar;
	_budget = 1000;
}


PlacementState::~PlacementState()
{
}

void PlacementState::Update(float deltaTime)
{
	_levelEdit.Update();

	//tempAddObj

	//T adds Trap
	if (_inputHandler->IsPressed(0x54))
	{
		int cost = 500;
		if (_budget - cost >= 0)
		{
			_levelEdit.Add(TRAP, TRAP);
			_budget -= cost;
		}
	}
}

void PlacementState::OnStateEnter()
{
	_levelEdit.Initialize(&_initVar);
}

void PlacementState::OnStateExit()
{

}


