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


Unit::Unit()
{
	_aStar = new AI::AStar();
	_visibleTiles = nullptr;
	_visionRadius = 0;
}

Unit::Unit(int x, int z, const Tilemap* tileMap)
{
	//_unitTilePosition = {x, z};
	_tileMap = tileMap;
	_aStar = new AI::AStar(_tileMap->GetWidth(), _tileMap->GetHeight(), {x, z}, {0,0}, AI::AStar::OCTILE);		//TODO: Find the unit's goal --Victor

	for (int i = 0; i < _tileMap->GetWidth(); i++)
	{
		for (int j = 0; j < _tileMap->GetHeight(); j++)
		{
			if (_tileMap->IsObjectiveOnTile(i,j))
			{
				_aStar->SetGoalPosition({i, j});
			}
			if (_tileMap->IsWallOnTile(i, j))
			{
				_aStar->SetTileCost({i, j}, -1);						//TODO: More thorough check depending on objects in the tile --Victor
			}
			else
			{
				_aStar->SetTileCost({i, j}, 1);
			}
		}
	}
	_aStar->FindPath();
	_path = _aStar->GetPath();
	_pathLength = _aStar->GetPathLength();
}

Unit::Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_visionRadius = 3;
	_visibleTiles = new AI::Vec2D[ (2 * _visionRadius) * (2 * _visionRadius)];
	_nrOfVisibleTiles = 0;

	_tileMap = tileMap;
	_aStar = new AI::AStar(_tileMap->GetWidth(), _tileMap->GetHeight(), _tilePosition, {0,0}, AI::AStar::OCTILE);		//TODO: Find the unit's goal --Victor

	//Scan tilemap for floor layout and objectives
	//CheckAllTiles();
}


Unit::~Unit()
{
	delete _aStar;
}


/*
	NOT IMPLEMENTED
	Checks tiles that are visible to the unit
*/
void Unit::CheckVisibleTiles()
{

}

void Unit::CheckAllTiles()
{
	for (int i = 0; i < _tileMap->GetWidth(); i++)
	{
		for (int j = 0; j < _tileMap->GetHeight(); j++)
		{
			if (_tileMap->IsObjectiveOnTile(i, j))
			{
				_aStar->SetGoalPosition({i, j});			//TODO: Make a proper decision in case of multiple goals --Victor
				_aStar->SetTileCost({ i, j }, 1);
			}
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

/*
	Checks pathfinding for an array of tiles leading to the goal.
	Cleaning the map is not fully implemented, which means changing paths midway won't work.
*/
void Unit::CalculatePath()
{
	_aStar->FindPath();
	_path = _aStar->GetPath();
	_pathLength = _aStar->GetPathLength() -1;
	AI::Vec2D nextTile = _path[--_pathLength];
	_direction = { nextTile._x, nextTile._y };
	_direction -= _tilePosition;
}

/*
	Calculate path to a predetermined goal
*/
void Unit::CalculatePath(AI::Vec2D goal)
{
	_goalTilePosition = goal;
	_aStar->cleanMap();
	_aStar->SetStartPosition(_tilePosition);
	_aStar->SetGoalPosition(goal);
	if (_aStar->FindPath())
	{
		_path = _aStar->GetPath();
		_pathLength = _aStar->GetPathLength();
		AI::Vec2D nextTile = _path[--_pathLength];
		_direction = {nextTile._x, nextTile._y};
		_direction -= _tilePosition;
	}
}

/*
	Moves the unit to the tile it's aiming for and selects a new walking direction.
	This should NOT update every frame. It only updates when the unit reaches a new tile. 
*/
void Unit::Move()
{
	if (_pathLength > 0)
	{
		_tilePosition += _direction;

		AI::Vec2D nextTile = _path[--_pathLength];
		_direction = {nextTile._x, nextTile._y};
		_direction -= _tilePosition;
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
}

void Unit::Update()
{
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
}

void Unit::Release()
{}

/*
	Gathers the tiles which are visible to the unit.
	Currently only checks if walls block vision (not traps or units)
*/
void Unit::FindVisibleTiles()
{
	double startSlope = 1.0;
	_visibleTiles[0] = AI::Vec2D(this->GetTilePosition()._x, this->GetTilePosition()._y);
	_nrOfVisibleTiles = 1;

	//If looking north, scan octant 1 and 2
	if (_direction._y == 1)
	{
		ScanOctant(1, 1, startSlope, 0.0);
		ScanOctant(1, 2, startSlope, 0.0);
	}
	//If looking east, scan octant 3 and 4
	else if (_direction._x == 1)
	{
		ScanOctant(1, 3, startSlope, 0.0);
		ScanOctant(1, 4, startSlope, 0.0);
	}
	//If looking south, scan octant 5 and 6
	else if (_direction._y == -1)
	{
		ScanOctant(1, 5, startSlope, 0.0);
		ScanOctant(1, 6, startSlope, 0.0);
	}
	//If looking west, scan octant 7 and 8
	else if (_direction._x == -1)
	{
		ScanOctant(1, 7, startSlope, 0.0);
		ScanOctant(1, 8, startSlope, 0.0);
	}
}

