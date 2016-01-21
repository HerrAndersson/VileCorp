#include "Unit.h"

void Unit::ScanOctant(int depth, int octant, double &startSlope, double endSlope)
{
	int visRangeSqrd = _visionRadius * _visionRadius;
	int x = 0;
	int y = 0;
	int unitPosX = this->GetTilePosition()._x;
	int unitPosY = this->GetTilePosition()._y;
	int leftToRight = 1;							//can be 1 or -1
	bool rowByRow = true;
	int endSlopeCompOffset = 1;
	int startSlopeOffset = 1;
	AI::Vec2D prevTileVec = AI::Vec2D(0, 0);
	AI::Vec2D endCornerVec = AI::Vec2D(0, 0);
	AI::Vec2D startCornerVec = AI::Vec2D(0, 0);

	switch (octant)
	{
	case 1:
		prevTileVec = AI::Vec2D(-1, 0);//In what direction the previous tile was
		endCornerVec = AI::Vec2D(-1, -1);//Is multiplied with 0.5
		startCornerVec = AI::Vec2D(-1, 1);//Is multiplied with 0.5

		y = unitPosY + depth;
		if (y < 0)
		{
			return;
		}
		x = unitPosX - (int)((startSlope * (double)depth));
		x = max(x, 0);
		break;
	case 2:
		leftToRight = -1;
		endSlopeCompOffset = -1;
		startSlopeOffset = -1;
		prevTileVec = AI::Vec2D(1, 0);
		endCornerVec = AI::Vec2D(1, -1);
		startCornerVec = AI::Vec2D(1, 1);

		y = unitPosY + depth;
		if (y >= _tileMap->GetHeight())
		{
			return;
		}
		x = unitPosX + (int)((startSlope * (double)depth));
		x = min(x, _tileMap->GetWidth() - 1);
		break;
	case 3:
		leftToRight = -1;
		rowByRow = false;
		endSlopeCompOffset = -1;
		startSlopeOffset = -1;
		prevTileVec = AI::Vec2D(0, 1);
		endCornerVec = AI::Vec2D(-1, 1);
		startCornerVec = AI::Vec2D(1, 1);

		x = unitPosX + depth;
		if (x >= _tileMap->GetWidth())
		{
			return;
		}
		y = unitPosY + (int)((startSlope * (double)depth));
		y = min(y, _tileMap->GetHeight() - 1);
		break;
	case 4:
		rowByRow = false;
		startSlopeOffset = -1;
		prevTileVec = AI::Vec2D(0, -1);
		endCornerVec = AI::Vec2D(-1, -1);
		startCornerVec = AI::Vec2D(1, -1);

		x = unitPosX + depth;
		if (x >= _tileMap->GetWidth())
		{
			return;
		}
		y = unitPosY - (int)((startSlope * (double)depth));
		y = max(y, 0);
		break;
	case 5:
		leftToRight = -1;
		startSlopeOffset = -1;
		prevTileVec = AI::Vec2D(-1, 0);
		endCornerVec = AI::Vec2D(1, -1);
		startCornerVec = AI::Vec2D(-1, -1);

		y = unitPosY - depth;
		if (y < 0)
		{
			return;
		}
		x = unitPosX + (int)((startSlope * (double)depth));
		x = min(x, _tileMap->GetWidth() - 1);
		break;
	case 6:
		endSlopeCompOffset = -1;
		startSlopeOffset = -1;
		prevTileVec = AI::Vec2D(-1, 0);
		endCornerVec = AI::Vec2D(1, -1);
		startCornerVec = AI::Vec2D(-1, -1);

		y = unitPosY - depth;
		if (y < 0)
		{
			return;
		}
		x = unitPosX - (int)((startSlope * (double)depth));
		x = max(x, 0);
		break;
	case 7:
		rowByRow = false;
		endSlopeCompOffset = -1;
		startSlopeOffset = -1;
		prevTileVec = AI::Vec2D(0, -1);
		endCornerVec = AI::Vec2D(1, -1);
		startCornerVec = AI::Vec2D(-1, -1);

		x = unitPosX - depth;
		if (x < 0)
		{
			return;
		}

		y = unitPosY - (int)((startSlope * (double)depth));
		y = max(y, 0);
		break;
	case 8:
		leftToRight = -1;
		rowByRow = false;
		prevTileVec = AI::Vec2D(0, 1);
		endCornerVec = AI::Vec2D(1, 1);
		startCornerVec = AI::Vec2D(-1, 1);

		x = unitPosX - depth;
		if (x < 0)
		{
			return;
		}
		y = unitPosY + (int)((startSlope * (double)depth));
		y = min(y, _tileMap->GetHeight() - 1);
		break;
	};

	//While within octant relevant scan area
	while ((endSlopeCompOffset)* GetSlope(x, y, unitPosX, unitPosY, (!rowByRow)) >= endSlope)
	{
		if (GetVisDistance(x, y, unitPosX, unitPosY) <= visRangeSqrd)
		{
			//Is there a wall on current tile
			if (_tileMap->IsWallOnTile(x, y))
			{
				//If previous tile was in range and not a wall
				if (_tileMap->IsValid(x, y) && !_tileMap->IsWallOnTile(x + prevTileVec._x, y + prevTileVec._y))
				{
					//Recurse and adjust depth with new end slope
					ScanOctant(depth + 1, octant, startSlope, (endSlopeCompOffset)* GetSlope(x + (endCornerVec._x * 0.5), y + (endCornerVec._y * 0.5), unitPosX, unitPosY, (!rowByRow)));
				}
			}
			else
			{
				if ((_tileMap->IsWallOnTile(x + (prevTileVec._x), y + (prevTileVec._y))))
				{
					startSlope = (startSlopeOffset)* GetSlope(x + (startCornerVec._x * 0.5), y + (startCornerVec._y * 0.5), unitPosX, unitPosY, (!rowByRow));
				}
				//Add current tile to visible
				_visibleTiles[_nrOfVisibleTiles] = AI::Vec2D(x, y);
				_nrOfVisibleTiles++;
			}
		}
		if (rowByRow)			//If row by row, use x
		{
			x += leftToRight;
		}
		else					//Else use y 
		{
			y += leftToRight;
		}
	}
	if (rowByRow)				//If row by row, use x
	{
		x -= leftToRight;
	}
	else						//Else use y
	{
		y -= leftToRight;
	}

	x = min(max(x, 0), _tileMap->GetWidth() - 1);	//Set x within tile range if it's outside
	y = min(max(y, 0), _tileMap->GetHeight() - 1);	//Set y within tile range if it's outside

	//Recurse and run if depth is within range
	if (depth < _visionRadius && (!_tileMap->IsWallOnTile(x, y)))
	{
		ScanOctant(depth + 1, octant, startSlope, endSlope);
	}
}


double Unit::GetSlope(double x1, double y1, double x2, double y2, bool invert)
{
	if (invert)
	{
		return -((y1 - y2) / (x1 - x2));
	}
	else
	{
		return -((x1 - x2) / (y1 - y2));
	}
}

int Unit::GetVisDistance(int x1, int y1, int x2, int y2)
{
	return (x1 - x2) * (x1 - x2) + ((y1 - y2) * (y1 - y2));
}

void Unit::CalculatePath()
{
	if (_aStar->FindPath())
	{
		_path = _aStar->GetPath();
		_pathLength = _aStar->GetPathLength();
		//_aStar->printMap();
	}
	else
	{
		_path = nullptr;
		_pathLength = 0;
		//	_aStar->printMap();

	}
}

int Unit::GetApproxDistance(AI::Vec2D target) const
{
	return _aStar->GetHeuristicDistance(_tilePosition, target);
}

Unit::Unit()
	: GameObject()
{
	_goalPriority = -1;
	_aStar = new AI::AStar();
	_visibleTiles = nullptr;
	_visionRadius = 0;
	_goalTilePosition = { 0,0 };
	_heldObject = nullptr;
	_objective = nullptr;
	_waiting = -1;
	_health = 1;
	_pathLength = 0;
	_path = nullptr;
}

Unit::Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_goalPriority = -1;
	_visionRadius = 3;
	_visibleTiles = new AI::Vec2D[(2 * _visionRadius) * (2 * _visionRadius)];
	_nrOfVisibleTiles = 0;
	_goalTilePosition = _tilePosition;
	_tileMap = tileMap;
	_aStar = new AI::AStar(_tileMap->GetWidth(), _tileMap->GetHeight(), _tilePosition, { 0,0 }, AI::AStar::OCTILE);
	_heldObject = nullptr;
	_objective = nullptr;
	_waiting = -1;
	_health = 1;					//TODO: Update constrcutor parameters to include health  --Victor
	_pathLength = 0;
	_path = nullptr;
}


Unit::~Unit()
{
	delete[] _visibleTiles;
	delete _aStar;
	_aStar = nullptr;
	//delete _heldObject;
	//_heldObject = nullptr;
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

int Unit::GetHealth()
{
	return _health;
}

GameObject * Unit::GetHeldObject() const
{
	return _heldObject;
}


/*
Gathers the tiles which are visible to the unit.
Currently only checks if walls block vision (not traps or units)
*/
void Unit::FindVisibleTiles()
{
	double startSlope = 1.0;
	_visibleTiles[0] = AI::Vec2D(this->GetTilePosition()._x, this->GetTilePosition()._y);
	_nrOfVisibleTiles = 1;

	int octant = 1;
	int c = 0;

	if (_direction._y == 1)
	{
		if (_direction._x == 1)
		{
			octant = 2;
		}
		else if (_direction._x == -1)
		{
			octant = 8;
		}
		else
		{
			octant = 1;
		}
	}
	else if (_direction._y == -1)
	{
		if (_direction._x == 1)
		{
			octant = 4;
		}
		else if (_direction._x == -1)
		{
			octant = 6;
		}
		else
		{
			octant = 5;
		}
	}
	else
	{
		if (_direction._x == 1)
		{
			octant = 3;
		}
		else
		{
			octant = 7;
		}
	}

	ScanOctant(1, octant, startSlope, 0.0);
	if (octant < 8)
	{
		octant++;
	}
	else
	{
		octant = 1;
	}
	startSlope = 1.0;
	ScanOctant(1, octant, startSlope, 0.0);
}

/*
Checks tiles that are visible to the unit
*/
void Unit::CheckVisibleTiles()
{
	for (int i = 0; i < _nrOfVisibleTiles; i++)
	{
		if (_tileMap->IsWallOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y))
		{
			_aStar->SetTileCost(_visibleTiles[i], -1);
		}
		else if (_tileMap->IsTrapOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y))											//TODO: Traps shouldn't be automatically visible --Victor
		{
			_aStar->SetTileCost(_visibleTiles[i], 10);
		}
		if (_tileMap->UnitsOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y) > 0 && !(_visibleTiles[i] == _goalTilePosition || _visibleTiles[i] == _tilePosition))	//Unit finds another unit
		{
			GameObject* unit = _tileMap->GetObjectOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y, ENEMY);
			if (unit == nullptr)
			{
				unit = _tileMap->GetObjectOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y, GUARD);
			}
			EvaluateTile(unit);				//
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
			if (_tileMap->IsWallOnTile(i, j))
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
			if (_tileMap->IsObjectiveOnTile(i, j))
			{
				_aStar->SetTileCost({ i, j }, 1);
				EvaluateTile(_tileMap->GetObjectOnTile(i, j, LOOT));
			}
			else if (_tileMap->IsTypeOnTile(i, j, SPAWN))
			{
				EvaluateTile(_tileMap->GetObjectOnTile(i, j, SPAWN));
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
	_objective = _tileMap->GetObjectOnTile(goal._x, goal._y, FLOOR);		//Note: Make sure walled tiles aren't valid goals
	_aStar->CleanMap();
	_aStar->SetStartPosition(_tilePosition);
	_aStar->SetGoalPosition(goal);
	CalculatePath();
}

void Unit::SetGoal(GameObject * objective)
{
	_goalTilePosition = objective->GetTilePosition();
	_objective = objective;
	_aStar->CleanMap();
	_aStar->SetStartPosition(_tilePosition);
	_aStar->SetGoalPosition(_goalTilePosition);
	CalculatePath();
}

/*
Moves the unit to the tile it's aiming for and selects a new walking direction.
This should NOT update every frame. It only updates when the unit reaches a new tile.

Name should be changed to make it clear that this is tile movement
*/
void Unit::Move()
{

	if (_pathLength <= 0)		//The unit has reached its goal and needs a new one
	{
		if (_objective != nullptr)
		{
			act(_objective);
		}
		CheckAllTiles();
	}
	//if (_goalTilePosition == _tilePosition)
	//{
	//	CheckAllTiles();
	//}
	_tilePosition += _direction;	

	if (_objective != nullptr && _objective->GetPickUpState() != ONTILE)			//Check that no one took your objective
	{
		_objective = nullptr;
		_pathLength = 0;														//reseting _pathLength to indicate that a new path needs to be found
	}

	//TODO: React to objects in same tile --Victor

	FindVisibleTiles();
	CheckVisibleTiles();
	if (_pathLength > 0)
	{
		AI::Vec2D nextTile = _path[--_pathLength];
		_direction = nextTile - _tilePosition;
	}
	else
	{
		_direction = {0,0};
		CheckAllTiles();
		Wait(60);
	}

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

void Unit::Update()
{

	if (_waiting > 0)
	{
		_waiting--;
	}
	else if (_waiting == 0)
	{
		_waiting--;
		Move();
	}
	else
	{
		if (_direction._x == 0 || _direction._y == 0)		//Right angle movement
		{
			_position.x += MOVE_SPEED * _direction._x;
			_position.z += MOVE_SPEED * _direction._y;
		}
		else												//Diagonal movement
		{
			_position.x += AI::SQRT2 * 0.5f * MOVE_SPEED * _direction._x;
			_position.z += AI::SQRT2 * 0.5f *MOVE_SPEED * _direction._y;
		}
		CalculateMatrix();
	}
}

void Unit::Release()
{}

void Unit::Wait(int frames)
{
	_waiting = frames;
}

void Unit::ChangeHealth(int damage)
{
	_health -= damage;
}



