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
	AI::Vec2D _nextTile;			//The tile it is currently walking to.
	AI::Vec2D _goalTilePosition;	//The tile it eventually wants to reach.
	AI::AStar* _aStar;
	AI::Vec2D* _path;
	int _pathLength;
	const Tilemap* _tileMap;		//Pointer to the tileMap in objectHandler(?). Units should preferably have read-, but not write-access.
	GameObject** _allLoot;
	int _nrOfLoot;
	GameObject** _allSpawnPoints;
	int _nrOfSpawnPoints;
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
	bool _isAtSpawn;
	
	//Movement state variables
	MoveState _moveState;
	bool _isSwitchingTile;			//Indicator to ObjectHandler that it should be moved to a different tile
	int _interactionTime;			//Timer for when it needs to do something.
	int _waiting;

	// Animations variables
	Anim _lastAnimState;

	void CalculatePath();								//Calls pathfiding algorithm and checks that a path was indeed found
	void Rotate();										//Rotation for model, game logic and vision cone
	int GetApproxDistance(AI::Vec2D target)const;		//The distance to a position assuming no obstacles. Used for picking a target.
	void SetGoal(AI::Vec2D goal);
	void SetGoal(GameObject* objective);				//Does the things necessary to change the pathfinding to a new goal
	void ExpandArray(GameObject** &arr, int &sizeOfArray);
public:
	Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, const Tilemap* tileMap, AI::Vec2D direction = { 1, 0 });
	virtual ~Unit();

	int GetPathLength()const;
	AI::Vec2D GetGoalTilePosition();
	AI::Vec2D GetDirection();
	AI::Vec2D GetNextTile()const;
	int GetHealth();
	GameObject* GetHeldObject()const;
	GameObject* GetObjective() const;
	MoveState GetMoveState()const;
	int GetVisionRadius()const;
	bool GetAnimisFinished();

	void SetGoalTilePosition(AI::Vec2D goal);													//Decides on a target and changes MoveState to FINDING_PATH
	void SetDirection(const AI::Vec2D& direction);
	void SetSwitchingTile(const bool switchTile);
	void SetVisibility(bool visible);
	void SetTilePosition(AI::Vec2D pos);
	void SetStatusEffect(StatusEffect effect, int intervalTime = 0, int totalTime = 0);			//set type of effect, duration of effect, and time between each activation

	bool IsSwitchingTile()const;

	//Decision making
	void CheckVisibleTiles();																	//Checks for targets in vision cone. Typically done after switching tile.
	void CheckAllTiles();																		//Checks the whole map. Typically done by idle enemies to find loot or an exit point.
	void InitializePathFinding();																//Transfers map info to the pathfinding. Done once the tilemap is fully loaded.
	virtual void EvaluateTile(System::Type objective, AI::Vec2D tile) = 0;
	virtual void EvaluateTile(GameObject* obj) = 0;												//Decide priority of potential targets
	virtual void Act(GameObject* obj) = 0;														//Act on target within range

	//Update related actions
	virtual void Update(float deltaTime);							//Checks MoveState for appropriate update function
	virtual void Moving();											//Update function when unit is not dead center on a tile.
	virtual void SwitchingNode();									//Update function when unit is on a tile center and needs to decide next tile to move to.
	void ClearObjective();											//Cleaning function for when objective is lost.
	virtual void Release();

	//Damage and status effects
	void ActivateStatus();											//Called whenever status triggers (once for everything but burning)
	void DeactivateStatus();										//Called when status wears off.
	void TakeDamage(int damage);
	bool GetIsAtSpawn() const;
	void SetIsAtSpawn(bool isAtSpawn);

	void Animate(Anim anim);
};

