#include "Unit.h"

void Unit::ScanOctant(int depth, int octant, double &startSlope, double endSlope)
{
	/*
	(The radius of the visual range)^2
	*/
	int visRangeSqrd = _visionRadius * _visionRadius;
	int x = 0;
	int y = 0;
	int unitPosX = this->GetTilePosition()._x;
	int unitPosY = this->GetTilePosition()._y;

	switch (octant)
	{
	case 1:
		y = unitPosY - depth;

		if (y < 0)
		{
			return;
		}

		x = unitPosX - (int)((startSlope * (double)depth) <= endSlope);
		if (x < 0)
		{
			x = 0;
		}

		while (GetSlope(x, y, unitPosX, unitPosY, false))
		{
			if (GetVisDistance(x, y, unitPosX, unitPosY) <= visRangeSqrd)
			{
				if (_tileMap->IsWallOnTile(x, y))//Is there a wall on current tile
				{
					if (x - 1 >= 0 && (!_tileMap->IsWallOnTile(x - 1, y)))//Is there not a wall on previous tile
					{
						//Recurse and adjust depth
						ScanOctant(depth + 1, octant, startSlope, GetSlope(x - 0.5, y + 0.5, unitPosX, unitPosY, false));
					}
				}
				else//Current cell NOT blocked
				{
					if (x - 1 >= 0 && (_tileMap->IsWallOnTile(x - 1, y)))//Previous tile in range AND blocked
					{
						//Adjust start slope
						startSlope = GetSlope(x - 0.5, y - 0.5, unitPosX, unitPosY, false);

						_visibleTiles[_nrOfVisibleTiles] = AI::Vec2D(x, y);
						_nrOfVisibleTiles++;
					}
				}
			}
			x++;
		}
		x--;
		break;

	case 2:
		y = unitPosY - depth;

		if (y < 0)
		{
			return;
		}

		x = unitPosX - (int)((startSlope * (double)depth));
		if (x >= _tileMap->GetWidth())
		{
			x = _tileMap->GetWidth() - 1;
		}

		while (GetSlope(x, y, unitPosX, unitPosY, false) <= endSlope)
		{
			if (GetVisDistance(x, y, unitPosX, unitPosY) <= visRangeSqrd)
			{
				//Is there a wall on current tile
				if (_tileMap->IsWallOnTile(x, y))
				{
					//Is there NOT a wall on next tile
					if (x + 1 < _tileMap->GetWidth() && (!_tileMap->IsWallOnTile(x + 1, y)))
					{
						//Recurse and adjust depth
						ScanOctant(depth + 1, octant, startSlope, GetSlope(x + 0.5, y + 0.5, unitPosX, unitPosY, false));
					}
				}
				//Current cell NOT blocked
				else
				{
					//Is x within map width and is there a wall on next tile
					if (x + 1 < _tileMap->GetWidth() && (_tileMap->IsWallOnTile(x + 1, y)))//Previous tile in range AND blocked
					{
						//Adjust start slope
						startSlope = -GetSlope(x + 0.5, y - 0.5, unitPosX, unitPosY, false);

						_visibleTiles[_nrOfVisibleTiles] = AI::Vec2D(x, y);
						_nrOfVisibleTiles++;
					}
				}
			}
			x--;
		}
		x++;
		break;

	case 3:
		x = unitPosX - depth;

		if (x >= _tileMap->GetWidth())
		{
			return;
		}

		y = unitPosY - (int)((startSlope * (double)depth));
		if (y < 0)
		{
			y = 0;
		}

		while (GetSlope(x, y, unitPosX, unitPosY, true) <= endSlope)
		{
			if (GetVisDistance(x, y, unitPosX, unitPosY) <= visRangeSqrd)
			{
				//Is there a wall on current tile
				if (_tileMap->IsWallOnTile(x, y))
				{
					//Is y in the map and isthere NOT a wall on previous tile
					if (y - 1 >= 0 && (!_tileMap->IsWallOnTile(x, y - 1)))
					{
						//Recurse and adjust depth
						ScanOctant(depth + 1, octant, startSlope, GetSlope(x - 0.5, y - 0.5, unitPosX, unitPosY, true));
					}
				}
				//Current cell NOT blocked
				else
				{
					//Is y within map and is there a wall on previous tile
					if (y - 1 >= 0 && (_tileMap->IsWallOnTile(x, y - 1)))//Previous tile in range AND blocked
					{
						//Adjust start slope
						startSlope = -GetSlope(x + 0.5, y - 0.5, unitPosX, unitPosY, true);

						_visibleTiles[_nrOfVisibleTiles] = AI::Vec2D(x, y);
						_nrOfVisibleTiles++;
					}
				}
			}
			y++;
		}
		y--;
		break;

	case 4:
		x = unitPosX + depth;

		if (x >= _tileMap->GetWidth())
		{
			return;
		}

		y = unitPosY + (int)((startSlope * (double)depth));
		if (y >= _tileMap->GetHeight())
		{
			y = _tileMap->GetHeight() - 1;
		}

		while (GetSlope(x, y, unitPosX, unitPosY, true) >= endSlope)
		{
			if (GetVisDistance(x, y, unitPosX, unitPosY) <= visRangeSqrd)
			{
				//Is there a wall on current tile
				if (_tileMap->IsWallOnTile(x, y))
				{
					//Is y in the map and isthere NOT a wall on previous tile
					if (y + 1 < _tileMap->GetHeight() && (!_tileMap->IsWallOnTile(x, y + 1)))
					{
						//Recurse and adjust depth
						ScanOctant(depth + 1, octant, startSlope, GetSlope(x - 0.5, y + 0.5, unitPosX, unitPosY, true));
					}
				}
				//Current cell NOT blocked
				else
				{
					//Is y within map and is there a wall on previous tile
					if (y + 1 < _tileMap->GetHeight() && (_tileMap->IsWallOnTile(x, y + 1)))//Previous tile in range AND blocked
					{
						//Adjust start slope
						startSlope = GetSlope(x + 0.5, y + 0.5, unitPosX, unitPosY, true);

						_visibleTiles[_nrOfVisibleTiles] = AI::Vec2D(x, y);
						_nrOfVisibleTiles++;
					}
				}
			}
			y--;
		}
		y++;
		break;

	case 5:
		y = unitPosY + depth;

		if (y >= _tileMap->GetHeight())
		{
			return;
		}

		x = unitPosX + (int)((startSlope * (double)depth));
		if (x >= _tileMap->GetWidth())
		{
			x = _tileMap->GetWidth() - 1;
		}

		while (GetSlope(x, y, unitPosX, unitPosY, false) >= endSlope)
		{
			if (GetVisDistance(x, y, unitPosX, unitPosY) <= visRangeSqrd)
			{
				//Is there a wall on current tile
				if (_tileMap->IsWallOnTile(x, y))
				{
					//Is y in the map and isthere NOT a wall on previous tile
					if (x + 1 < _tileMap->GetHeight() && (!_tileMap->IsWallOnTile(x + 1, y)))
					{
						//Recurse and adjust depth
						ScanOctant(depth + 1, octant, startSlope, GetSlope(x + 0.5, y - 0.5, unitPosX, unitPosY, false));
					}
				}
				//Current cell NOT blocked
				else
				{
					//Is y within map and is there a wall on previous tile
					if (x + 1 < _tileMap->GetHeight() && (_tileMap->IsWallOnTile(x + 1, y)))//Previous tile in range AND blocked
					{
						//Adjust start slope
						startSlope = GetSlope(x + 0.5, y + 0.5, unitPosX, unitPosY, false);

						_visibleTiles[_nrOfVisibleTiles] = AI::Vec2D(x, y);
						_nrOfVisibleTiles++;
					}
				}
			}
			x--;
		}
		x++;
		break;

	case 6:
		y = unitPosY + depth;

		if (y >= _tileMap->GetHeight())
		{
			return;
		}

		x = unitPosX - (int)((startSlope * (double)depth));
		if (x < 0)
		{
			x = 0;
		}

		while (GetSlope(x, y, unitPosX, unitPosY, false) <= endSlope)
		{
			if (GetVisDistance(x, y, unitPosX, unitPosY) <= visRangeSqrd)
			{
				//Is there a wall on current tile
				if (_tileMap->IsWallOnTile(x, y))
				{
					//Is y in the map and isthere NOT a wall on previous tile
					if (x - 1 >= 0 && (!_tileMap->IsWallOnTile(x - 1, y)))
					{
						//Recurse and adjust depth
						ScanOctant(depth + 1, octant, startSlope, GetSlope(x - 0.5, y - 0.5, unitPosX, unitPosY, false));
					}
				}
				//Current cell NOT blocked
				else
				{
					//Is y within map and is there a wall on previous tile
					if (x - 1 >= 0 && (_tileMap->IsWallOnTile(x - 1, y)))//Previous tile in range AND blocked
					{
						//Adjust start slope
						startSlope = -GetSlope(x - 0.5, y + 0.5, unitPosX, unitPosY, false);

						_visibleTiles[_nrOfVisibleTiles] = AI::Vec2D(x, y);
						_nrOfVisibleTiles++;
					}
				}
			}
			x++;
		}
		x--;
		break;
	case 7:
		x = unitPosX - depth;

		if (x < 0)
		{
			return;
		}

		y = unitPosY + (int)((startSlope * (double)depth));
		if (y >= _tileMap->GetHeight())
		{
			y = _tileMap->GetHeight() - 1;
		}

		while (GetSlope(x, y, unitPosX, unitPosY, true) <= endSlope)
		{
			if (GetVisDistance(x, y, unitPosX, unitPosY) <= visRangeSqrd)
			{
				//Is there a wall on current tile
				if (_tileMap->IsWallOnTile(x, y))
				{
					//Is y in the map and isthere NOT a wall on previous tile
					if (y + 1 < _tileMap->GetHeight() && (!_tileMap->IsWallOnTile(x, y + 1)))
					{
						//Recurse and adjust depth
						ScanOctant(depth + 1, octant, startSlope, GetSlope(x + 0.5, y + 0.5, unitPosX, unitPosY, true));
					}
				}
				//Current cell NOT blocked
				else
				{
					//Is y within map and is there a wall on previous tile
					if (y + 1 < _tileMap->GetHeight() && (_tileMap->IsWallOnTile(x, y + 1)))//Previous tile in range AND blocked
					{
						//Adjust start slope
						startSlope = -GetSlope(x - 0.5, y + 0.5, unitPosX, unitPosY, true);

						_visibleTiles[_nrOfVisibleTiles] = AI::Vec2D(x, y);
						_nrOfVisibleTiles++;
					}
				}
			}
			y--;
		}
		y++;
		break;

	case 8:
		x = unitPosX - depth;

		if (x < 0)
		{
			return;
		}

		y = unitPosY - (int)((startSlope * (double)depth));
		if (y < 0)
		{
			y = 0;
		}

		while (GetSlope(x, y, unitPosX, unitPosY, true) >= endSlope)
		{
			if (GetVisDistance(x, y, unitPosX, unitPosY) <= visRangeSqrd)
			{
				//Is there a wall on current tile
				if (_tileMap->IsWallOnTile(x, y))
				{
					//Is y in the map and isthere NOT a wall on previous tile
					if (y - 1 >= 0 && (!_tileMap->IsWallOnTile(x, y - 1)))
					{
						//Recurse and adjust depth
						ScanOctant(depth + 1, octant, startSlope, GetSlope(x + 0.5, y - 0.5, unitPosX, unitPosY, true));
					}
				}
				//Current cell NOT blocked
				else
				{
					//Is y within map and is there a wall on previous tile
					if (y - 1 >= 0 && (_tileMap->IsWallOnTile(x, y - 1)))//Previous tile in range AND blocked
					{
						//Adjust start slope
						startSlope = -GetSlope(x - 0.5, y - 0.5, unitPosX, unitPosY, true);

						_visibleTiles[_nrOfVisibleTiles] = AI::Vec2D(x, y);
						_nrOfVisibleTiles++;
					}
				}
			}
			y++;
		}
		y--;
		break;
	};
}


double Unit::GetSlope(double x1, double y1, double x2, double y2, bool invert)
{
	if (invert)
	{
		return (y1 - y2) / (x1 - x2);
	}
	else
	{
		return (x1 - x2) / (y1 - y2);
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
	}
	else
	{
		_path = nullptr;
		_pathLength = 0;
	}
}


int Unit::GetApproxDistance(AI::Vec2D target) const
{
	return static_cast<int>(_aStar->GetHeuristicDistance(_tilePosition, target));
}

Unit::Unit()
	: GameObject()
{
	_goalPriority = -1;
	_aStar = new AI::AStar();
	_visibleTiles = nullptr;
	_visionRadius = 0;
	_goalTilePosition = {0,0};
}

Unit::Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_goalPriority = -1;
	_visionRadius = 3;
	_visibleTiles = new AI::Vec2D[ (2 * _visionRadius + 1) * (2 * _visionRadius + 1)];
	_nrOfVisibleTiles = 0;
	_goalTilePosition = _tilePosition;
	_tileMap = tileMap;
	_aStar = new AI::AStar(_tileMap->GetWidth(), _tileMap->GetHeight(), _tilePosition, {0,0}, AI::AStar::OCTILE);		//TODO: Find the unit's goal --Victor

	//Scan tilemap for floor layout and objectives
	//CheckAllTiles();
}


Unit::~Unit()
{
	delete[] _visibleTiles;
	delete _aStar;
	_aStar = nullptr;
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



/*
Gathers the tiles which are visible to the unit.
Currently only checks if walls block vision (not traps or units)
*/
//void Unit::FindVisibleTiles()
//{
//	double startSlope = 1.0;
//	_visibleTiles[0] = AI::Vec2D(this->GetTilePosition()._x, this->GetTilePosition()._y);
//	_nrOfVisibleTiles = 1;
//
//	//If looking north, scan octant 1 and 2
//	if (_direction._y == 1)
//	{
//		ScanOctant(1, 1, startSlope, 0.0);
//		ScanOctant(1, 2, startSlope, 0.0);
//	}
//	//If looking east, scan octant 3 and 4
//	else if (_direction._x == 1)
//	{
//		ScanOctant(1, 3, startSlope, 0.0);
//		ScanOctant(1, 4, startSlope, 0.0);
//	}
//	//If looking south, scan octant 5 and 6
//	else if (_direction._y == -1)
//	{
//		ScanOctant(1, 5, startSlope, 0.0);
//		ScanOctant(1, 6, startSlope, 0.0);
//	}
//	//If looking west, scan octant 7 and 8
//	else if (_direction._x == -1)
//	{
//		ScanOctant(1, 7, startSlope, 0.0);
//		ScanOctant(1, 8, startSlope, 0.0);
//	}
//} 

/*
Gathers the tiles which are visible to the unit.
Temporary solution until the proper vision cone is fixed.
*/
void Unit::FindVisibleTiles()
{
	_nrOfVisibleTiles = 0;
	short temp = _ID;
	for (int i = 0; i < 2 * _visionRadius + 1; i++)
	{
		for (int j = 0; j < 2 * _visionRadius + 1; j++)
		{
			_visibleTiles[_nrOfVisibleTiles++] = {_tilePosition._x + i - _visionRadius, _tilePosition._y + j - _visionRadius};
		}
	}

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
		//if (_ID == 100)									//Temporary solution to only use one of the units
		//{
		//	if (_tileMap->IsTypeOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y, UNIT) && !(_visibleTiles[i] == _goalTilePosition || _visibleTiles[i] == _tilePosition))	//Unit finds another unit
		//	{
		//		CalculatePath(_visibleTiles[i]);
		//	}
		//}
		if (_tileMap->UnitsOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y) > 0 && !(_visibleTiles[i] == _goalTilePosition || _visibleTiles[i] == _tilePosition))	//Unit finds another unit
		{
			EvaluateTile(UNIT, _visibleTiles[i]);			//TODO: Change unit to guard/enemy --Victor
		}
	}
}

void Unit::CheckAllTiles()
{
	for (int i = 0; i < _tileMap->GetWidth(); i++)
	{
		for (int j = 0; j < _tileMap->GetHeight(); j++)
		{ 
			//Handle objectives
			if (_tileMap->IsObjectiveOnTile(i, j))
			{
				//if (_tilePosition == _goalTilePosition || _aStar->GetHeuristicDistance(_tilePosition, {i, j}) < _aStar->GetHeuristicDistance(_tilePosition, _goalTilePosition))	//Choose the 'closest' objective
				//{
				//	_goalTilePosition = {i, j};
				//	_aStar->SetGoalPosition(_goalTilePosition);
				//}
				_aStar->SetTileCost({ i, j }, 1);
				EvaluateTile(LOOT, {i, j});
			}
			//Handle walls
 			if (_tileMap->IsWallOnTile(i, j))
			{
				_aStar->SetTileCost({i, j}, -1);
			}
			else
			{
				_aStar->SetTileCost({i, j}, 1);
			}
		}
	}
}

void Unit::SetGoal(AI::Vec2D goal)
{
	_goalTilePosition = goal;
	_aStar->CleanMap();
	_aStar->SetStartPosition(_tilePosition);
	_aStar->SetGoalPosition(goal);
	CalculatePath();

}

/*
	Calculate path to a predetermined goal
*/
void Unit::CalculatePath(AI::Vec2D goal)
{
	_goalTilePosition = goal;
	_aStar->CleanMap();
	_aStar->SetStartPosition(_tilePosition);
	_aStar->SetGoalPosition(goal);
	if (_aStar->FindPath())
	{
		_path = _aStar->GetPath();
		_pathLength = _aStar->GetPathLength();
	}
	else
	{
		_path = nullptr;
		_pathLength = 0;
	}

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
		CheckAllTiles();
	}

	_tilePosition += _direction;

	//TODO: React to objects in same tile --Victor

	FindVisibleTiles();
	CheckVisibleTiles();

	AI::Vec2D nextTile = _path[--_pathLength];
	_direction = nextTile - _tilePosition;
	if (_direction._x == 0)
	{
 		_rotation.y = DirectX::XM_PIDIV2 * (_direction._y + 1);
		//_rotation.y = atan(_direction._y / _direction._x);
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
	if (_direction._x == 0)
	{
	}
	if (_pathLength > 0)
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
	else
	{
		CheckAllTiles();
		CalculatePath(_goalTilePosition);
	}
}

void Unit::Release()
{}


