#include "PlacementState.h"

bool compareFloat3(XMFLOAT3 a, XMFLOAT3 b)
{
	if (a.x == b.x && a.z == b.z)
	{
		return true;
	}
	return false;
}

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
			GameObject* selected = _objectHandler->Find(TRAP, _levelEdit.GetSelectedObject());
			vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(TRAP);
			
			if (vec->empty())
			{
				_levelEdit.Add(TRAP, TRAP);
				_budget -= cost;
			}
			else
			{
				bool taken = false;
				for (GameObject* g : *vec)
				{
					if (selected != g && compareFloat3(selected->GetPosition(), g->GetPosition()))
					{
						taken = true;
						break;
					}
				}
				if (!taken)
				{
					_levelEdit.Add(TRAP, TRAP);
					_budget -= cost;
				}
			}
		}
	}
	else if (_inputHandler->IsPressed(System::Input::Backspace))
	{
		GameObject* selected = _objectHandler->Find(TRAP, _levelEdit.GetSelectedObject());
		vector<GameObject*>* vec = &_objectHandler->GetGameObjects()->at(TRAP);
		for (GameObject* g : *vec)
		{
			if (selected != g && compareFloat3(selected->GetPosition(), g->GetPosition()))
			{
				_budget += cost;
				_objectHandler->Remove(/*TRAP,*/ g->GetID());
				break;
			}
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


