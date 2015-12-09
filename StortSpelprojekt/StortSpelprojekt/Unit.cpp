#include "Unit.h"



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
	_visionRadius = 3;
	_visibleTiles = new AI::Vec2D[ (2 * _visionRadius) * (2 * _visionRadius)];
	nrOfVisibleTiles = 0;
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
Gathers the tiles which are visible to the unit.
The temporary solution is to check all tiles in radius.
Eventually, it should check a cone, with walls blocking the unit's vision.
*/
void Unit::FindVisibleTiles()
{
	nrOfVisibleTiles = 0;
	short temp = _ID;
	for (int i = 0; i < 2 * _visionRadius + 1; i++)
	{
		for (int j = 0; j < 2 * _visionRadius + 1; j++)
		{
			_visibleTiles[nrOfVisibleTiles++] = {_tilePosition._x + i - _visionRadius, _tilePosition._y + j - _visionRadius};
		}
	}

}

/*
	Checks tiles that are visible to the unit
*/
void Unit::CheckVisibleTiles()
{
	for (int i = 0; i < nrOfVisibleTiles; i++)
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
