#pragma once
#include "GameObject.h"
#include "../Tilemap.h"
#include "AStar.h"
#include "../VisionCone.h"
#include <DirectXMath.h>
#include <stdlib.h>
#include <time.h> 
class Unit : public GameObject
{
public:
	enum MoveState {IDLE, FINDING_PATH, MOVING, SWITCHING_NODE, AT_OBJECTIVE, FLEEING};
	enum StatusEffect{NO_EFFECT, BURNING, SLOWED, STUNNED, SCARED, CONFUSED};
	enum Anim{IDLEANIM, WALKANIM, FIGHTANIM, PICKUPOBJECTANIM, FIXTRAPANIM, DISABLETRAPANIM, HURTANIM, DEATHANIM, NR_OF_ANIM/*Has to be last*/};
protected:
	//Pathfinding variables
	AI::Vec2D _nextTile;
	AI::AStar* _aStar;
	AI::Vec2D _goalTilePosition;
	AI::Vec2D* _path;
	const Tilemap* _tileMap;		//Pointer to the tileMap in objectHandler(?). Units should preferably have read-, but not write-access.
	int _pathLength;
	GameObject* _objective;
	GameObject* _heldObject;
	int _goalPriority;				//Lower value means higher priority

	//Unit stats
	float  _moveSpeed;
	int _health;
	int _baseDamage;
	int _visionRadius;
	VisionCone* _visionCone;
	int _trapInteractionTime;		//Multiplier that affects repair time for guards, or disarm time for enemies.

	//Unit status
	StatusEffect _status;
	int _statusInterval;			//Time between status activations
	int _statusTimer;				//Time until the status ends
	
	//Movement state variables
	MoveState _moveState;
	bool _isSwitchingTile;
	int _interactionTime;
	int _waiting;

	// Animations variables
	Anim _lastAnimState;

	void CalculatePath();
	void Rotate();
	int GetApproxDistance(AI::Vec2D target)const;
	void SetGoal(AI::Vec2D goal);
	void SetGoal(GameObject* objective);
public:
	Unit();
	Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, const Tilemap* tileMap);
	virtual ~Unit();

	int GetPathLength()const;
	AI::Vec2D GetGoalTilePosition();
	AI::Vec2D GetDirection();
	AI::Vec2D GetNextTile()const;
	int GetHealth();
	GameObject* GetHeldObject()const;
	MoveState GetMoveState()const;
	int GetVisionRadius()const;
	bool GetAnimisFinished();

	void SetGoalTilePosition(AI::Vec2D goal);
	void SetDirection(const AI::Vec2D direction);
	void SetSwitchingTile(const bool switchTile);
	void SetVisibility(bool visible);
	void SetTilePosition(AI::Vec2D pos);
	void SetStatusEffect(StatusEffect effect, int intervalTime = 0, int totalTime = 0);			//set type of effect, duration of effect, and time between each activation

	bool IsSwitchingTile()const;

	//Decision making
	void CheckVisibleTiles();
	void CheckAllTiles();
	void InitializePathFinding();
	virtual void EvaluateTile(System::Type objective, AI::Vec2D tile) = 0;
	virtual void EvaluateTile(GameObject* obj) = 0;
	virtual void Act(GameObject* obj) = 0;

	//Update related actions
	virtual void Update(float deltaTime);
	virtual void Moving();
	virtual void SwitchingNode();									//context specific action on the unit's objective
	void ClearObjective();
	virtual void Release();

	//Damage and status effects
	void ActivateStatus();
	void DeactivateStatus();
	void TakeDamage(int damage);

	void Animate(Anim anim);
};

