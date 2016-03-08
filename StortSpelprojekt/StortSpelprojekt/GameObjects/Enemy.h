#pragma once
#include "Trap.h"
#include "Unit.h"


const int TIME_TO_HIDE = 60;		//frames needed to stop being visible
enum EnemyType{BASICENEMY, DISABLER, ASSASSIN};
class Enemy : public Unit
{
private:
	int _visibilityTimer;			//becomes hidden if staying out of sight long enough
	int _detectionSkill;			//Chance to detect traps
	int _disarmSkill;				//Chance to deactivate traps without getting caught by them.
	Unit* _pursuer;

	void Flee();
	bool SafeToAttack(AI::Vec2D dir);
	bool TryToDisarm(Trap* trap);
	bool SpotTrap(Trap* trap);
	void DisarmTrap(Trap* trap);
public:
	Enemy(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, Renderer::ParticleEventQueue* particleEventQueue, const Tilemap* tileMap, const int enemyType, AI::Vec2D direction = { 1, 0 });
	virtual ~Enemy();
	void EvaluateTile(System::Type objective, AI::Vec2D tile);
	void EvaluateTile(GameObject* obj);
	void Act(GameObject* obj);
	void Release();
	void Update(float deltaTime);
	void ResetVisibilityTimer();
	void Moving();
};