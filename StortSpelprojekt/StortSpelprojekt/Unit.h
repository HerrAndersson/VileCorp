#pragma once
#include "GameObjectComp.h"
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

	int _healthLethal;
	int _healthNonLethal;

public:
	Unit();
	Unit(int x, int z, const Tilemap* tileMap);
	~Unit();

	// Input negative value to decrease lethal health
	void ChangeHealthLethal(int change);

	// Input negative value to decrease nonlethal health
	void ChangeHealthNonLethal(int change);

	void Move();
	void Update();
	void Release();
};

