#pragma once
#include "GameObject.h"
#include "../Tilemap.h"
#include "AStar.h"
#include "../VisionCone.h"
#include <DirectXMath.h>
class Unit : public GameObject
{
public:
	const float MOVE_SPEED = 0.025f;				//Movement per frame

	enum MoveState {IDLE, FINDING_PATH, MOVING, SWITCHING_NODE, AT_OBJECTIVE, FLEEING};
	enum StatusEffect{NO_EFFECT, BURNING, SLOWED, STUNNED, SCARED, CONFUSED};
protected:
	AI::Vec2D _nextTile;
	AI::AStar* _aStar;
	AI::Vec2D _goalTilePosition;
	AI::Vec2D* _path;
	int _pathLength;
	GameObject* _objective;
	int _goalPriority;				//Lower value means higher priority
	float  _moveSpeed;
	const Tilemap* _tileMap;		//Pointer to the tileMap in objectHandler(?). Units should preferably have read-, but not write-access.
				
	int _visionRadius;
	VisionCone* _visionCone;

	int _waiting;

	int _health;
	bool _isSwitchingTile;
	float _speedMultiplier;

	int _interactionTime;
	GameObject* _heldObject;

	MoveState _moveState;

	void CalculatePath();
	void Rotate();
	int GetApproxDistance(AI::Vec2D target)const;
	void SetGoal(AI::Vec2D goal);
	void SetGoal(GameObject* objective);
public:
	Unit();
	Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, System::SoundModule* soundModule, const Tilemap* tileMap);
	virtual ~Unit();
	int GetPathLength()const;
	AI::Vec2D GetGoalTilePosition();
	void SetGoalTilePosition(AI::Vec2D goal);
	AI::Vec2D GetDirection();
	void SetDirection(const AI::Vec2D direction);
	AI::Vec2D GetNextTile()const;
	int GetHealth();
	GameObject* GetHeldObject()const;
	MoveState GetMoveState()const;

	bool IsSwitchingTile()const;
	void SetSwitchingTile(const bool switchTile);

	void CheckVisibleTiles();
	void CheckAllTiles();
	void InitializePathFinding();
	virtual void EvaluateTile(Type objective, AI::Vec2D tile) = 0;
	virtual void EvaluateTile(GameObject* obj) = 0;

	virtual void Update(float deltaTime);
	virtual void Release();
	virtual void Act(GameObject* obj) = 0;									//context specific action on the unit's objective
	void ClearObjective();

	void TakeDamage(int damage);

	void SetVisibility(bool visible);
	void UseCountdown(int frames = 0);
	int GetVisionRadius()const;

	void SetTilePosition(AI::Vec2D pos);
	virtual void Moving();
	virtual void SwitchingNode();
	
	enum Anim { IDLEANIM, WALKANIM, FIXTRAPANIM, FIGHTANIM, PICKUPOBJECTANIM, NR_OF_ANIM/*Has to be last*/ };
	void Animate(Anim anim);
	bool GetAnimisFinished();

};

