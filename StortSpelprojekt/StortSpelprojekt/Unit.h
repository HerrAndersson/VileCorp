#pragma once
#include "GameObject.h"
#include "Tilemap.h"
#include "AStar.h"
class Unit : public GameObject
{
private:
	AI::AStar* _aStar;
	AI::Vec2D _unitTilePosition;
	AI::Vec2D _goalTilePosition;
	AI::Vec2D* _path;
	int _pathLength;
	const Tilemap* _tileMap;		//Pointer to the tileMap in objectHandler(?). Units should preferably have read-, but not write-access.

	float  _moveSpeed;
public:
	Unit();
	Unit(int x, int z, const Tilemap* tileMap);
	~Unit();
	void Move();
	void Update();
	void Release();
	
};

