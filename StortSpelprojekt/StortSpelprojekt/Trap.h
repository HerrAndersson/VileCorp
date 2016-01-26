#pragma once
#include "GameObject.h"
#include "Unit.h"

enum TrapType{ SPIKE, TESLACOIL, SHARK};


class Trap : public GameObject
{
private:
	TrapType _trapType;
	bool _isActive;
	int _damage;

	AI::Vec2D _direction;

	const Tilemap* _tileMap;
	AI::Vec2D* _areaOfEffect;
	int _nrOfAOETiles;
	int _areaOfEffectArrayCapacity;

	void CalculateCircleAOE(int radius);
	void CalculateLineAOE(int length);
	bool isUnblocked(AI::Vec2D pos);
	AI::Vec2D convertOctant(int octant, AI::Vec2D pos, bool in = true);
public:
	Trap();
//	Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject);
	Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, 
		 TrapType trapType = SPIKE, AI::Vec2D direction = {0,0});
	virtual ~Trap();

	void Activate(Unit* target);
	void Update(float deltaTime);
	void Release();
};

