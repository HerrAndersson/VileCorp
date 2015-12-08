#include "Unit.h"



Unit::Unit()
{
	_aStar = new AI::AStar();
	_visibleTiles = nullptr;
	_visionRadius = 0;
}

Unit::Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_visionRadius = 3;
	_visibleTiles = new AI::Vec2D[ (2 * _visionRadius) * (2 * _visionRadius)];
	nrOfVisibleTiles = 0;

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
	for (int i = 0; i < 2 * _visionRadius + 1; i++)
	{
		for (int j = 0; j < 2 * _visionRadius + 1; j++)
		{
			_visibleTiles[nrOfVisibleTiles++] = AI::Vec2D(_tilePosition._x + i - _visionRadius, _tilePosition._y + i - _visionRadius);
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
		if (_tileMap->IsWallOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y))
		{
			_aStar->SetTileCost(_visibleTiles[i], -1);
		}
		else if (_tileMap->IsTrapOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y))											//TODO: Traps shouldn't be automatically visible --Victor
		{
			_aStar->SetTileCost(_visibleTiles[i], 10);
		}
		else if (_tileMap->IsTypeOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y, UNIT))									//Unit finds another unit
		{			
			if (_visibleTiles[i] != _goalTilePosition)																		//Goal should be updated to chase unit
			{
				if (_visibleTiles[i]._x - _goalTilePosition._x > 1 || _visibleTiles[i]._y - _goalTilePosition._y > 1)		//Goal has moved more than one tile
				{
					CalculatePath(_visibleTiles[i]);
				}
				else												//Goal has only moved one tile, which is appended on _path. Might get a bit weird if target is faster than you.
				{
					AI::Vec2D* temp = new AI::Vec2D[++_pathLength];
					for (int i = 0; i < _pathLength; i++)
					{
						temp[i] = _path[i];
					}
					delete[] _path;
					_path = temp;
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
	_aStar->CleanMap();
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

	Name should be changed to make it clear that this is tile movement
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

		FindVisibleTiles();
		CheckVisibleTiles();
	}
	else									//You reached your goal. Find a new one
	{

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
