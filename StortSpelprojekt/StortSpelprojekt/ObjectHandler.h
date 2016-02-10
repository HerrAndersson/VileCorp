#pragma once
#include <vector>
#include "stdafx.h"
#include "GameObject.h"
#include "Unit.h"
#include "Trap.h"
#include "Architecture.h"
#include "SpawnPoint.h"
#include "Enemy.h"
#include "Guard.h"
#include "Tilemap.h"
#include "JsonStructs.h"
#include "AssetManager.h"
#include "StateMachine\States.h"
#include "Spotlight.h"
#include "Pointlight.h"
#include "Grid.h"
#include "LightCulling.h"

/*
ObjectHandler
Used for updating gamelogic and rendering.

Stores pointers of all GameObjects in a std::vector

Handles GameObjects

Can add and remove GameObject pointers from the std::vector
Finds objects based on their objectID or vector index.
Finds an object and return its vector index
Returns all objects of a certain Type (i.e. Traps) as a seperate objectHandler
*/


//A struct containing 1 renderobject and all the gameobjects' matrices that uses said renderobject
struct RenderList
{
	RenderObject* _renderObject;
	vector<XMMATRIX> _modelMatrices;
};

class ObjectHandler
{
private:

	vector<vector<GameObject*>> _gameObjects;
	GameObjectInfo* _gameObjectInfo;
	void ActivateTileset(const string& name);
	Tilemap* _tilemap;
	Grid* _buildingGrid;

	int _idCount = 0;
	int _objectCount = 0;

	AssetManager* _assetManager;
	ID3D11Device* _device;

	map<GameObject*, Renderer::Spotlight*> _spotlights;
	map<GameObject*, Renderer::Pointlight*> _pointligths;
	LightCulling* _lightCulling;

	Architecture*	MakeFloor(GameObjectFloorInfo* data, const XMFLOAT3& position, const XMFLOAT3& rotation);
	Architecture*	MakeWall(GameObjectWallInfo* data, const XMFLOAT3& position, const XMFLOAT3& rotation);
	Architecture*	MakeLoot(GameObjectLootInfo* data, const XMFLOAT3& position, const XMFLOAT3& rotation);
	SpawnPoint*		MakeSpawn(GameObjectSpawnInfo* data, const XMFLOAT3& position, const XMFLOAT3& rotation);
	Trap*			MakeTrap(GameObjectTrapInfo* data, const XMFLOAT3& position, const XMFLOAT3& rotation, const int subIndex = 0);
	Guard*			MakeGuard(GameObjectGuardInfo* data, const XMFLOAT3& position, const XMFLOAT3& rotation);
	Enemy*			MakeEnemy(GameObjectEnemyInfo* data, const XMFLOAT3& position, const XMFLOAT3& rotation);

	void ReleaseGameObjects();

public:

	ObjectHandler(ID3D11Device* device, AssetManager* assetManager, GameObjectInfo* data);
	~ObjectHandler();

	//Add a gameobject
	bool Add(Type type, int index, const XMFLOAT3& position, const XMFLOAT3& rotation, const int subIndex = 0);
	bool Add(Type type, const std::string& name, const XMFLOAT3& position, const XMFLOAT3& rotation, const int subIndex = 0);
	
	bool Remove(int ID);
	bool Remove(Type type, int ID);
	bool Remove(GameObject* gameObject);

	GameObject* Find(int ID);
	GameObject* Find(Type type, int ID);
	GameObject* Find(Type type, short index);

	//Returns a vector containing all gameobjects with the same type
	vector<GameObject*> GetAllByType(Type type);
	//Returns a list of a renderobject and matrices for all objects using the renderobject
	RenderList GetAllByType(int renderObjectID);
	vector<vector<GameObject*>>* GetGameObjects();

	map<GameObject*, Renderer::Spotlight*>* GetSpotlights();
	map<GameObject*, Renderer::Pointlight*>* GetPointlights();
	vector<vector<GameObject*>>* GetObjectsInLight(Renderer::Spotlight* spotlight);

	GameObjectInfo* GetBlueprints();

	int GetObjectCount() const;

	Tilemap* GetTileMap() const;
	void SetTileMap(Tilemap* tilemap);
	void MinimizeTileMap();
	void EnlargeTilemap(int offset);
	Grid* GetBuildingGrid();

	bool LoadLevel(int lvlIndex);

	void InitPathfinding();
	void EnableSpawnPoints();
	void DisableSpawnPoints();

	//Update gamelogic of all objects
	void Update(float deltaTime);
	//Relase all object resources
	void Release();
};

