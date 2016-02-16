#include "Unit.h"

void Unit::CalculatePath()
{
	if (_aStar->FindPath())
	{
		_path = _aStar->GetPath();
		_pathLength = _aStar->GetPathLength();
		for (int i = 0; i < _pathLength; i++)
		{
			/*if (_tileMap->IsFloorOnTile(_path[i]))
			{
				_tileMap->GetObjectOnTile(_path[i], FLOOR)->SetColorOffset({0,4,0});
			}*/
		}

		if (_stop == false)
		{
			Animate(WALKANIM);
		}
		_isMoving = true;
		_direction = _path[--_pathLength] - _tilePosition;
		Rotate();
		//_aStar->printMap();
	}
	else
	{
		_path = nullptr;
		_pathLength = 0;
		//	_aStar->printMap();
	}
}

void Unit::Rotate()
{
	if (_direction._x != 0 || _direction._y != 0)
	{
		if (_direction._x == 0)
		{
			_rotation.y = DirectX::XM_PIDIV2 * (_direction._y + 1);
		}
		else if (_direction._x == -1)
		{
			_rotation.y = DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4 * _direction._y;
		}
		else
		{
			_rotation.y = 3 * DirectX::XM_PIDIV2 - DirectX::XM_PIDIV4 * _direction._y;
		}
		CalculateMatrix();
	}
	_visionCone->ColorVisibleTiles({0,0,0});
	_visionCone->FindVisibleTiles(_tilePosition, _direction);
	_visionCone->ColorVisibleTiles({0,0,3});
}

int Unit::GetApproxDistance(AI::Vec2D target) const
{
	return (int)_aStar->GetHeuristicDistance(_tilePosition, target);
}


/*
	Decides the best direction to run away from a foe.
*/
void Unit::Flee()
{
	if (_pursuer == nullptr || !_visible)		//TODO Add other conditions to stop fleeing --Victor
	{
		_isFleeing = false;
		CheckAllTiles();
		Wait(20);
	}
	else
	{
		AI::Vec2D offset = _tilePosition - _pursuer->GetTilePosition();
		AI::Vec2D bestDir = {0,0};
		float bestDist = 0;
		float tempDist = 0;
		for (int i = 0; i < 8; i++)
		{
		//	AI::Vec2D tempDist = offset + AI::NEIGHBOUR_OFFSETS[i];

			if (i < 4)						//weighting to normalize diagonal and straight directions
			{
				tempDist = pow(offset._x + AI::SQRT2 * AI::NEIGHBOUR_OFFSETS[i]._x, 2) + pow(offset._y + AI::SQRT2 * AI::NEIGHBOUR_OFFSETS[i]._y, 2);
			}
			else
			{
				tempDist = pow(offset._x + AI::NEIGHBOUR_OFFSETS[i]._x, 2) + pow(offset._y + AI::NEIGHBOUR_OFFSETS[i]._y, 2);
			}

			if (_tileMap->IsFloorOnTile(_tilePosition + AI::NEIGHBOUR_OFFSETS[i]) && !_tileMap->IsEnemyOnTile(_tilePosition + AI::NEIGHBOUR_OFFSETS[i]) && tempDist > bestDist)
			{
				bestDist = tempDist;
				bestDir = AI::NEIGHBOUR_OFFSETS[i];
			}
		}
		_direction = bestDir;
		Rotate();
	}
}

Unit::Unit()
	: GameObject()
{
	_isFleeing = false;
	_goalPriority = -1;
	_aStar = new AI::AStar();
	_visionCone = nullptr;
	_visionRadius = 0;
	_goalTilePosition = { 0,0 };
	_heldObject = nullptr;
	_objective = nullptr;
	_waiting = -1;
	_health = 1;
	_pathLength = 0;
	_path = nullptr;
	_isMoving = false;
	_direction = {0, -1};
	_trapInteractionTime = -1;
	Rotate();
}

Unit::Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_isFleeing = false;
	_goalPriority = -1;
	_visionRadius = 6;
	_goalTilePosition = _tilePosition;
	_tileMap = tileMap;
	_visionCone = new VisionCone(_visionRadius, _tileMap);
	_aStar = new AI::AStar(_tileMap->GetWidth(), _tileMap->GetHeight(), _tilePosition, { 0,0 }, AI::AStar::OCTILE);
	_heldObject = nullptr;
	_objective = nullptr;
	_waiting = -1;
	_health = 1;					//TODO: Update constructor parameters to include health  --Victor
	_pathLength = 0;
	_path = nullptr;
	_isMoving = false;
	_stop = false;
	_direction = {0, 1};
	Rotate();
	if (_renderObject->_isSkinned)
	{
		_animation = new Animation(_renderObject->_skeleton, true);
		_animation->Freeze(false);
	}
	_trapInteractionTime = -1;
}

Unit::~Unit()
{
	//delete[] _visibleTiles;
	delete _aStar;
	_aStar = nullptr;
	delete _visionCone;
	//delete _heldObject;
	//_heldObject = nullptr;
	if (_animation != nullptr)
	{
		delete _animation;
	}
}

int Unit::GetPathLength() const
{
	return _pathLength;
}

AI::Vec2D Unit::GetGoal()
{
	return _goalTilePosition;
}

AI::Vec2D Unit::GetDirection()
{
	return _direction;
}

void Unit::SetDirection(const AI::Vec2D direction)
{
	_direction = direction;
	_visionCone->FindVisibleTiles(_tilePosition, _direction);
}

int Unit::GetHealth()
{
	return _health;
}

GameObject * Unit::GetHeldObject() const
{
	return _heldObject;
}


/*
Checks tiles that are visible to the unit
*/
void Unit::CheckVisibleTiles()
{
	AI::Vec2D* visibleTiles = _visionCone->GetVisibleTiles();
	for (int i = 0; i < _visionCone->GetNrOfVisibleTiles(); i++)
	{
		//if (_tileMap->IsWallOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y))
		//{
		//	_aStar->SetTileCost(_visibleTiles[i], -1);
		//}
		if (_tileMap->IsTrapOnTile(visibleTiles[i]._x, visibleTiles[i]._y))											//TODO: Traps shouldn't be automatically visible --Victor
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], TRAP));
		}

		if (_type != ENEMY && _tileMap->IsEnemyOnTile(visibleTiles[i]))
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], ENEMY));
		}
		if (_type != GUARD && _tileMap->IsGuardOnTile(visibleTiles[i]))
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], GUARD));
		}
		if (_tileMap->IsObjectiveOnTile(visibleTiles[i]))
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], LOOT));
		}
	}
}

void Unit::CheckAllTiles()
{
	for (int i = 0; i < _tileMap->GetWidth(); i++)
	{
		for (int j = 0; j < _tileMap->GetHeight(); j++)
		{
			//Handle walls
			if (_tileMap->IsWallOnTile(AI::Vec2D(i, j)) || _tileMap->IsFurnitureOnTile(AI::Vec2D(i, j)))
			{
				_aStar->SetTileCost({ i, j }, -1);
			}
			else
			{
				_aStar->SetTileCost({ i, j }, 1);
			}
		}
	}
	for (int i = 0; i < _tileMap->GetWidth(); i++)
	{
		for (int j = 0; j < _tileMap->GetHeight(); j++)
		{
			//Handle objectives
			if (_tileMap->IsObjectiveOnTile(AI::Vec2D(i, j)))
			{
				_aStar->SetTileCost({ i, j }, 1);
				EvaluateTile(_tileMap->GetObjectOnTile(AI::Vec2D(i, j), LOOT));
			}
			else if (_tileMap->IsTypeOnTile(AI::Vec2D(i, j), SPAWN))
			{
				EvaluateTile(_tileMap->GetObjectOnTile(AI::Vec2D(i, j), SPAWN));
			}
		}
	}
	//_aStar->printMap();
}

/*
Moves the goal and finds the path to the new goal
*/
void Unit::SetGoal(AI::Vec2D goal)
{
 	_goalTilePosition = goal;
	if (_tileMap->IsTrapOnTile(goal))
	{
		_objective = _tileMap->GetObjectOnTile(goal, TRAP);
	}
	else
	{
		_objective = _tileMap->GetObjectOnTile(goal, FLOOR);		//Note: Make sure walled tiles aren't valid goals
	}
	//_objective->SetColorOffset({5,0,0});
	if (_objective != nullptr)
	{
		if (_objective->InRange(_tilePosition))
		{
			act(_objective);
		}
		else
		{
			_aStar->CleanMap();
			_aStar->SetStartPosition(_tilePosition);
			_aStar->SetGoalPosition(goal);
			CalculatePath();
		}
	}
}

void Unit::SetGoal(GameObject * objective)
{
	_goalTilePosition = objective->GetTilePosition();
	_objective = objective;
	if (_objective->InRange(_tilePosition))
	{
		act(_objective);
	}
	else
	{
		_aStar->CleanMap();
		_aStar->SetStartPosition(_tilePosition);
		_aStar->SetGoalPosition(_goalTilePosition);
		CalculatePath();
	}
}

/*
Moves the unit to the tile it's aiming for and selects a new walking direction.
This should NOT update every frame. It only updates when the unit reaches a new tile.

Name should be changed to make it clear that this is tile movement
*/
void Unit::Move()
{
	bool foundNextTile = false;
	if (_isMoving)
	{
		//	_tileMap->GetObjectOnTile(_tilePosition, FLOOR)->SetColorOffset({0,0,0});
		_tilePosition += _direction;
		//	_tileMap->GetObjectOnTile(_tilePosition, FLOOR)->SetColorOffset({0,4,0});
	}
	if (_objective != nullptr && _objective->GetPickUpState() != ONTILE)			//Check that no one took your objective
	{
		_objective = nullptr;
		_pathLength = 0;														//reseting _pathLength to indicate that a new path needs to be found
	}

	if (_isFleeing)
	{
		Flee();
	}
	else if (_pathLength > 0)
	{
		_isMoving = true;
		foundNextTile = true;
		AI::Vec2D nextTile = _path[--_pathLength];
		_direction = nextTile - _tilePosition;
	}
	if (!_isFleeing && (!foundNextTile || (_objective != nullptr && _objective->InRange(_tilePosition))))
	{
		Animate(IDLEANIM);
		_isMoving = false;
		if (_objective != nullptr && _objective->InRange(_tilePosition))
		{
			act(_objective);
		}
		//_direction = {0,0};
		CheckAllTiles();
		Wait(10);
	}
	Rotate();
	CheckVisibleTiles();
}

void Unit::Update(float deltaTime)
{
	if (_animation != nullptr)
	{
		_animation->Update(deltaTime);
		if (_animation->GetisFinished())
		{
			_stop = false;
		}
	}
	if (_trapInteractionTime >= 0)
	{
		UseTrap();
	}
	else if(_stop == false)
	{
		if (_waiting > 0)
		{
			_waiting--;
		}
		else if (_waiting == 0 && !_isMoving)
		{
			_waiting--;
			Move();
		}
		if (_isMoving)
		{
			if (_direction._x == 0 || _direction._y == 0)		//Right angle movement
			{
				_position.x += MOVE_SPEED * _direction._x;
				_position.z += MOVE_SPEED * _direction._y;
			}
			else if (_direction._x == 0 && _direction._y == 0)
			{
				CheckVisibleTiles();
			}
			else												//Diagonal movement
			{
				_position.x += AI::SQRT2 * 0.5f * MOVE_SPEED * _direction._x;
				_position.z += AI::SQRT2 * 0.5f *MOVE_SPEED * _direction._y;
			}
			CalculateMatrix();
		}
		/*if (_tileMap->IsFloorOnTile(_tilePosition))
		{
			_tileMap->GetObjectOnTile(_tilePosition, FLOOR)->SetColorOffset({0,0,4});
		}*/
	}
}

void Unit::Release()
{}

void Unit::Wait(int frames)
{
	_waiting = frames;
}

void Unit::ClearObjective()
{
	_objective = nullptr;
	_path = nullptr;
	_pathLength = 0;
}

void Unit::TakeDamage(int damage)
{
	if (_health > 0)
	{
		_health -= damage;
	}
}

void Unit::SetVisibility(bool visible)
{
	GameObject::SetVisibility(visible);
	if (_heldObject != nullptr)
	{
		_heldObject->SetVisibility(visible);
	}
}

void Unit::UseTrap()
{

	if (_trapInteractionTime < 0)
	{
		_trapInteractionTime = 60;
	}
	else if (_trapInteractionTime > 0)
	{
		_trapInteractionTime--;
	}
	else
	{
		act(_objective);
		_trapInteractionTime--;
	}
}

int Unit::GetVisionRadius() const
{
	return _visionRadius;
}

bool Unit::GetAnimisFinished()
{
	return _animation->GetisFinished();
}

void Unit::Animate(Anim anim)
{
	if (_renderObject->_isSkinned)
	{
		if (_renderObject->_type == GUARD)
		{
			switch (anim)
			{
			case IDLEANIM:
				_animation->SetActionAsCycle(0, 1.0f, false);
				break;
			case WALKANIM:
				_animation->SetActionAsCycle(1, 1.0f, false);
				break;
			case FIXTRAPANIM:
				_animation->SetActionAsCycle(2, 1.0f, false);
				break;
			case FIGHTANIM:
				_animation->PlayAction(4, 3.5f, false, false);
				break;
			default:
				break;
			}
		}
		if (_renderObject->_type == ENEMY)
		{
			switch (anim)
			{
			case IDLEANIM:
				_animation->SetActionAsCycle(0, 1.0f, false);
				break;
			case WALKANIM:
				_animation->SetActionAsCycle(1, 1.0f, false);
				break;
			case FIGHTANIM:
				_animation->PlayAction(1, 1.0f, false, false);
				break;
			case PICKUPOBJECTANIM:
				_animation->PlayAction(3, 1.0f, false, false);
				break;
			default:
				break;
			}
		}
	}
}
