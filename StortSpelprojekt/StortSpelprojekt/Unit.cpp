#include "Unit.h"



Unit::Unit()
{
	_aStar = new AI::AStar();
}

Unit::Unit(int x, int z, const Tilemap* tileMap)
{
	_unitTilePosition = {x, z};
	_tileMap = tileMap;
	_aStar = new AI::AStar(_tileMap->GetWidth(), _tileMap->GetHeight(), _unitTilePosition, {0,0}, AI::AStar::OCTILE);		//TODO: Find the unit's goal --Victor

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


Unit::~Unit()
{
	delete _aStar;
}

void Unit::Move()
{
	_unitTilePosition = _path[--_pathLength];				//Temporary. Units shouldn't move one tile per frame.
}

void Unit::Update()
{
	Move();
	//TODO: update map within line of sight.
}

void Unit::Release()
{}
