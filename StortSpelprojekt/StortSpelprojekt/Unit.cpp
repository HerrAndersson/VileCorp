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
	
	bool leftToRight = true;
	bool rowByRow = true;
	int endSlopeCompOffset = 1;
	int startSlopeOffset = 1;
	AI::Vec2D prevTileVec = AI::Vec2D(0, 0);
	AI::Vec2D endCornerVec = AI::Vec2D(0, 0);
	AI::Vec2D startCornerVec = AI::Vec2D(0, 0);

	switch (octant)
	{
	case 1:
		leftToRight = true;
		rowByRow = true;
		endSlopeCompOffset = 1;//Negative of positive offset
		startSlopeOffset = 1;//Negative or positive offset
		prevTileVec = AI::Vec2D(-1, 0);//In what direction the previous tile was
		endCornerVec = AI::Vec2D(-1, -1);//Is multiplied with 0.5
		startCornerVec = AI::Vec2D(-1, 1);//Is multiplied with 0.5

		y = unitPosY + depth;
		if (y < 0)
		{
			return;
		}

		x = unitPosX - (int)((startSlope * (double)depth));
		if (x < 0)
		{
			x = 0;
		}
		break;

	case 2:
		leftToRight = false;
		rowByRow = true;
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
		if (x >= _tileMap->GetWidth())
		{
			x = _tileMap->GetWidth() - 1;
		}
		break;
		
	case 3:
		leftToRight = false;
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
		if (y >= _tileMap->GetHeight())
		{
			y = _tileMap->GetHeight() - 1;
		}
		break;

	case 4:
		leftToRight = true;
		rowByRow = false;
		endSlopeCompOffset = 1;
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
		if (y < 0)
		{
			y = 0;
		}

		break;

	case 5:
		leftToRight = false;
		rowByRow = true;
		endSlopeCompOffset = 1;
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
		if (x >= _tileMap->GetWidth())
		{
			x = _tileMap->GetWidth() - 1;
		}
		break;

	case 6:
		leftToRight = true;
		rowByRow = true;
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
		if (x < 0)
		{
			x = 0;
		}
		break;

	case 7:
		leftToRight = true;
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
		if (y < 0)
		{
			y = 0;
		}
		break;

	case 8:
		leftToRight = false;
		rowByRow = false;
		endSlopeCompOffset = 1;
		startSlopeOffset = 1;
		prevTileVec = AI::Vec2D(0, 1);
		endCornerVec = AI::Vec2D(1, 1);
		startCornerVec = AI::Vec2D(-1, 1);

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
		break;
	};

	//While within octant relevant scan area
	while ((endSlopeCompOffset) * GetSlope(x, y, unitPosX, unitPosY, (!rowByRow)) >= endSlope)
	{
		if (GetVisDistance(x, y, unitPosX, unitPosY) <= visRangeSqrd)
		{
			//Is there a wall on current tile
			if (_tileMap->IsWallOnTile(x, y))
			{
				//If previous tile was in range and not a wall
				if ((x + (prevTileVec._x) >= 0 && y + (prevTileVec._y) >= 0) && 
					(x + (prevTileVec._x) <= _tileMap->GetWidth() && y + (prevTileVec._y) <= _tileMap->GetHeight()))
				{
					if ((!_tileMap->IsWallOnTile(x + (prevTileVec._x), y + (prevTileVec._y))))
					{
						//Recurse and adjust depth with new end slope
						ScanOctant(depth + 1, octant, startSlope, 
							(endSlopeCompOffset) * GetSlope(x + (endCornerVec._x * 0.5), y + (endCornerVec._y * 0.5), unitPosX, unitPosY, (!rowByRow)));
					}
				}
			}
			//Current cell NOT blocked
			else
			{
				//If previous tile was in range and a wall
				if ((x + (prevTileVec._x) >= 0 && y + (prevTileVec._y) >= 0) &&
					(x + (prevTileVec._x) <= _tileMap->GetWidth() && y + (prevTileVec._y) <= _tileMap->GetHeight()))
				{
					if ((_tileMap->IsWallOnTile(x + (prevTileVec._x), y + (prevTileVec._y))))
					{
						startSlope = (startSlopeOffset) * GetSlope(x + (startCornerVec._x * 0.5), y + (startCornerVec._y * 0.5), unitPosX, unitPosY, (!rowByRow));
					}
				}

				//Add current tile to visible
				_visibleTiles[_nrOfVisibleTiles] = AI::Vec2D(x, y);
				_nrOfVisibleTiles++;
			}
		}

		//If row by row, use x
		if (rowByRow)
		{
			if (leftToRight)//If left to right, add to x
				x++;
			else//Else subtract x
				x--;
		}
		else//Else use y 
		{
			if (leftToRight)//If left to right, add to y
				y++;
			else//Else subtract y
				y--;
		}
	}
	
	if (rowByRow)//If row by row, use x
	{
		if (leftToRight)//If NOT left to right, add to x
			x--;
		else//Else subtract x
			x++;
	}
	else//Else use y
	{
		if (leftToRight)//If NOT left to right, add to y
			y--;
		else//Else subtract y
			y++;
	}



	//Set x within tile range if it's outside
	if (x < 0)
	{
		x = 0;
	}
	else if (x >= _tileMap->GetWidth())
	{
		x = this->_tileMap->GetWidth() - 1;
	}

	//Set y within tile range if it's outside
	if (y < 0)
	{
		y = 0;
	}
	else if (y >= _tileMap->GetHeight())
	{
		y = _tileMap->GetHeight() - 1;
	}

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


Unit::Unit()
	: GameObject()
{
	_aStar = new AI::AStar();
	_visibleTiles = nullptr;
	_visionRadius = 0;
	_goalTilePosition = {0,0};
}

Unit::Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_visionRadius = 10;
	_visibleTiles = new AI::Vec2D[ (2 * _visionRadius) * (2 * _visionRadius)];
	_nrOfVisibleTiles = 0;
	_goalTilePosition = _tilePosition;
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
	Checks tiles that are visible to the unit
*/
void Unit::CheckVisibleTiles()
{
	for (int i = 0; i < _nrOfVisibleTiles; i++)
	{
		if (i != 24)			//Skip the tile unit is on
		{
			if (_tileMap->IsWallOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y))
			{
				_aStar->SetTileCost(_visibleTiles[i], -1);
			}
			else if (_tileMap->IsTrapOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y))											//TODO: Traps shouldn't be automatically visible --Victor
			{
				_aStar->SetTileCost(_visibleTiles[i], 10);
			}
			if (_ID == 100)									//Temporary solution to only use one of the units
			{
				if (_tileMap->IsTypeOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y, UNIT) && _visibleTiles[i] != _goalTilePosition)	//Unit finds another unit
				{
					CalculatePath(_visibleTiles[i]);
				}
			}
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
				if (_tilePosition == _goalTilePosition || _aStar->GetHeuristicDistance(_tilePosition, {i, j}) < _aStar->GetHeuristicDistance(_tilePosition, _goalTilePosition))	//Choose the 'closest' objective
				{
					_goalTilePosition = {i, j};
					_aStar->SetGoalPosition(_goalTilePosition);
				}
				_aStar->SetTileCost({ i, j }, 1);
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

/*
	Checks pathfinding for an array of tiles leading to the goal.
	Cleaning the map is not fully implemented, which means changing paths midway won't work.
*/
void Unit::CalculatePath()
{
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
	//	AI::Vec2D nextTile = _path[--_pathLength];
	//	_direction = nextTile - _tilePosition;
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
		startSlope = 1.0;
		ScanOctant(1, 2, startSlope, 0.0);
	}
	//If looking east, scan octant 3 and 4
	else if (_direction._x == 1)
	{
		ScanOctant(1, 3, startSlope, 0.0);
		startSlope = 1.0;
		ScanOctant(1, 4, startSlope, 0.0);
	}
	//If looking south, scan octant 5 and 6
	else if (_direction._y == -1)
	{
		ScanOctant(1, 5, startSlope, 0.0);
		startSlope = 1.0;
		ScanOctant(1, 6, startSlope, 0.0);
	}
	//If looking west, scan octant 7 and 8
	else if (_direction._x == -1)
	{
		ScanOctant(1, 7, startSlope, 0.0);
		startSlope = 1.0;
		ScanOctant(1, 8, startSlope, 0.0);
	}
}
