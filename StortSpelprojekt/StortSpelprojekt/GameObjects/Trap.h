#pragma once
#include "GameObject.h"
#include "Unit.h"
#include <memory>

enum TrapType{ SPIKE, TESLACOIL, SHARK, GUN};


class Trap : public GameObject
{
private:
	int _cost;
	bool _isActive;
	bool _isVisibleToEnemies;
	int _damage;

	Unit::StatusEffect _statusEffect;
	int _statusTimer;					//Time the status lasts
	int _statusInterval;				//time between status effects. Set to same time as _statusTimer to only activate once.
	int _detectDifficulty;
	int _disarmDifficulty;

	AI::Vec2D* _occupiedTiles;
	int _tileSize;

	const Tilemap* _tileMap;
	AI::Vec2D* _areaOfEffect;
	int _nrOfAOETiles;
	int _areaOfEffectArrayCapacity;

	int _animLength[4];

	int CalculateCircle(int radius, AI::Vec2D basePosition, AI::Vec2D* arr);
	int CalculateLine(int length, AI::Vec2D basePosition, AI::Vec2D* arr);
	int CalculateRectangle(int length, int width, AI::Vec2D basePosition, AI::Vec2D* arr);
	bool IsUnblocked(AI::Vec2D pos);
	AI::Vec2D ConvertOctant(int octant, AI::Vec2D pos, bool in = true);

	void Initialize(int damage, int tileSize, int AOESize, int detectDifficulty, int disarmDifficulty,
					Unit::StatusEffect statusEffect, int statusTimer, int statusInterval);							/*Initialize adds any characteristics specific to the subtype*/
	void SetTiles();
public:
	Trap();
	Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, 
		 const Tilemap* tileMap, int trapType = SPIKE, AI::Vec2D direction = {-1,0}, int cost = 0);
	virtual ~Trap();

	AI::Vec2D* GetTiles()const;
	int GetTileSize()const;
	int GetDetectionDifficulty()const;
	int GetDisarmDifficulty()const;
	bool InRange(AI::Vec2D pos) const;
	bool IsTrapActive() const;
	void SetTrapActive(bool active);

	void Activate();
	void Update(float deltaTime);
	void Release();

	bool IsVisibleToEnemies()const;
	void SetVisibleToEnemies(bool visible);

	// Overloaded function
	void SetTilePosition(AI::Vec2D pos);
	AI::Vec2D GetDirection();
	void SetDirection(const AI::Vec2D direction);

	enum Anim { IDLEANIM, ACTIVATEANIM, DISABLEANIM, FIXANIM, NR_OF_ANIM/*Has to be last*/ };
	void Animate(Anim anim);
};

