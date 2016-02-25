#pragma once
#include "GameObject.h"
#include "Unit.h"
#include <memory>

enum TrapType{ SPIKE, TESLACOIL, SHARK, GUN};


class Trap : public GameObject
{
private:
	bool _isActive;
	bool _isVisibleToEnemies;
	int _damage;
	int _detectDifficulty;
	int _disarmDifficulty;

	AI::Vec2D* _occupiedTiles;
	int _tileSize;

	const Tilemap* _tileMap;
	AI::Vec2D* _areaOfEffect;
	int _nrOfAOETiles;
	int _areaOfEffectArrayCapacity;

	int CalculateCircle(int radius, AI::Vec2D basePosition, AI::Vec2D* arr);
	int CalculateLine(int length, AI::Vec2D basePosition, AI::Vec2D* arr);
	int CalculateRectangle(int length, int width, AI::Vec2D basePosition, AI::Vec2D* arr);
	bool IsUnblocked(AI::Vec2D pos);
	AI::Vec2D ConvertOctant(int octant, AI::Vec2D pos, bool in = true);

	void Initialize(int damage, int tileSize, int AOESize, int detectDifficulty, int disarmDifficulty);
	void SetTiles();
public:
	Trap();
	Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, System::SoundModule* soundModule,
		 const Tilemap* tileMap, int trapType = SPIKE, AI::Vec2D direction = {-1,0});
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

	enum Anim { IDLE, ACTIVATE, NR_OF_ANIM/*Has to be last*/ };
	void Animate(Anim anim);

	//Sound
	void PlayActivateSound();
};

