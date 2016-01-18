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

	int cost = 200;

	//T adds Trap
	if (_inputHandler->IsPressed(0x54))
	{	
		if (_budget - cost >= 0)
		{
			vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(TRAP);
			if (vec->empty())
			{
				_levelEdit.Add(TRAP, TRAP);
				_budget -= cost;
			}
			else
			{
				bool taken = false;
				if (_levelEdit.Marked(TRAP))
				{
					taken = true;
				}
				if (!taken)
				{
					_levelEdit.Add(TRAP, TRAP);
					_budget -= cost;
				}
			}
		}
	}
	else if (_inputHandler->IsPressed(System::Input::Delete))
	{
		if (!_levelEdit.Delete(TRAP))
		{
			_budget += cost;
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


