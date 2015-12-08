#pragma once
#include "GameObject.h"
#include "Tilemap.h"
#include "AStar.h"
class Unit : public GameObject
{
public:
	const float MOVE_SPEED = 0.02f;
private:
	AI::AStar* _aStar;
	//AI::Vec2D _unitTilePosition;				//replaced by _tilePosition in GameObject
	AI::Vec2D _goalTilePosition;
	AI::Vec2D* _path;
	int _pathLength;
	const Tilemap* _tileMap;		//Pointer to the tileMap in objectHandler(?). Units should preferably have read-, but not write-access.
	AI::Vec2D _direction;
	float  _moveSpeed;
	float _animTime;
public:
	Unit();
	Unit(int x, int z, const Tilemap* tileMap);
	Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap);
	~Unit();
	void CheckVisibleTiles();
	void CheckAllTiles();
	void CalculatePath();
	void CalculatePath(AI::Vec2D goal);
	void Move();
	void Update(float deltaTime);
	void Release();
	
};

