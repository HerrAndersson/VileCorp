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
	AI::Vec2D* _visibleTiles;
	int _nrOfVisibleTiles;

	int _health;

	void ScanOctant(int depth, int octant, double &startSlope, double endSlope);
	double GetSlope(double x1, double y1, double x2, double y2, bool invert);
	int GetVisDistance(int x1, int y1, int x2, int y2);
	void CalculatePath();

protected:
	int _goalPriority;				//Lower value means higher priority
	int GetApproxDistance(AI::Vec2D target)const;

public:
	Unit();
	Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap);
	virtual ~Unit();
	int GetPathLength()const;
	AI::Vec2D GetGoal();
	AI::Vec2D GetDirection();
	int GetHealth();
	void FindVisibleTiles();									 
	void CheckVisibleTiles();		
	void CheckAllTiles();				
	virtual void EvaluateTile(Type objective, AI::Vec2D tile) = 0;
	void SetGoal(AI::Vec2D goal);
	void Move();
	void Update();					
	virtual void Release();
	virtual void act(GameObject* obj) = 0;									//context specific action on the unit's objective
	void ChangeHealth(int damage);

};

