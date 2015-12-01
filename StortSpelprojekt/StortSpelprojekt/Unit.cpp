#include "Unit.h"



Unit::Unit()
{
	_aStar = new AI::AStar();
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
	_tileMap = tileMap;
	_aStar = new AI::AStar(_tileMap->GetWidth(), _tileMap->GetHeight(), _tilePosition, {0,0}, AI::AStar::OCTILE);		//TODO: Find the unit's goal --Victor

	//Scan tilemap for floor layout and objectives
	for (int i = 0; i < _tileMap->GetWidth(); i++)
	{
		for (int j = 0; j < _tileMap->GetHeight(); j++)
		{
			if (_tileMap->IsObjectiveOnTile(i, j))
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

/*
	Moves the unit to the tile it's aimning for and selects a new walking direction.
	This should NOT update every frame. It only updates when the unit reaches a new tile. 
*/
void Unit::Move()
{
	_tilePosition += _direction;

	AI::Vec2D nextTile = _path[--_pathLength];
	_direction = {nextTile._x, nextTile._y};
	_direction -= _tilePosition;
}

void Unit::Update()
{
	//Move();
	//TODO: update map within line of sight.
}

void Unit::Release()
{}
