#pragma once
#include "GameObject.h"
#include "Tilemap.h"
#include "AStar.h"
#include <DirectXMath.h>
class Unit : public GameObject
{
public:
	const float MOVE_SPEED = 0.02f;				//Movement per frame
private:
	AI::AStar* _aStar;
	AI::Vec2D _goalTilePosition;
	AI::Vec2D* _path;
	int _pathLength;
	const Tilemap* _tileMap;		//Pointer to the tileMap in objectHandler(?). Units should preferably have read-, but not write-access.
	AI::Vec2D _direction;
	float  _moveSpeed;

	int _visionRadius;
	int nrOfVisibleTiles;
	AI::Vec2D* _visibleTiles;
	int _nrOfVisibleTiles;

	void ScanOctant(int depth, int octant, double &startSlope, double endSlope);
	double GetSlope(double x1, double y1, double x2, double y2, bool invert);
	int GetVisDistance(int x1, int y1, int x2, int y2);

public:
	Unit();
	Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap);
	~Unit();
	int getPathLength()const;
	AI::Vec2D getGoal();
	AI::Vec2D getDirection();
	void FindVisibleTiles();
	void CheckVisibleTiles();
	void CheckAllTiles();
	void CalculatePath();
	void CalculatePath(AI::Vec2D goal);
	void Move();
	void Update();
	void Release();

};

