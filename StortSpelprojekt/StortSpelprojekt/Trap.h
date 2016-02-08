#pragma once
#include "GameObject.h"
#include "Unit.h"

enum TrapType{ SPIKE, TESLACOIL, SHARK};


class Trap : public GameObject
{
private:

	int _cost;
	TrapType _trapType;
	bool _isActive;
	int _damage;

	AI::Vec2D* _occupiedTiles;
	int _tileSize;

	const Tilemap* _tileMap;
	AI::Vec2D* _areaOfEffect;
	int _nrOfAOETiles;
	int _areaOfEffectArrayCapacity;

	void CalculateCircleAOE(int radius);
	void CalculateLineAOE(int length, AI::Vec2D direction);
	bool IsUnblocked(AI::Vec2D pos);
	AI::Vec2D ConvertOctant(int octant, AI::Vec2D pos, bool in = true);
	void Initialize(int damage, int tileSize, int AOESize);

public:

	Trap();
	Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, 
		 const Tilemap* tileMap, int trapType = SPIKE, AI::Vec2D direction = {1,0}, int cost = 0);
	virtual ~Trap();

	AI::Vec2D* GetTiles()const;
	int GetTileSize()const;

	void Activate();
	void Update(float deltaTime);
	void Release();
};

