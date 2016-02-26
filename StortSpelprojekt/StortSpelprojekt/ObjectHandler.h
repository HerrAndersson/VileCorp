#pragma once

#include "../System/SoundModule.h"
#include <vector>
#include "stdafx.h"
#include "GameObject.h"
#include "Unit.h"
#include "Trap.h"
#include "SecurityCamera.h"
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
#include "Settings/Settings.h"
#include "LightCulling.h"
#include "Blueprints.h"
#include "ParticleSystem\ParticleUtils.h"
#include "ParticleSystem\ParticleEventQueue.h"

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
	System::Settings* _settings;
	vector<vector<GameObject*>> _gameObjects;
	Blueprints _blueprints;
	GameObjectInfo* _gameObjectInfo;
	Tilemap* _tilemap;
	Grid* _buildingGrid;
	Level::LevelHeader _currentLevelHeader;
	System::SoundModule*	_soundModule;

	int _idCount = 0;
	int _objectCount = 0;
	string _levelfolder;

	AssetManager* _assetManager;
	ID3D11Device* _device;

	map<GameObject*, Renderer::Spotlight*> _spotlights;
	map<GameObject*, Renderer::Pointlight*> _pointligths;
	LightCulling* _lightCulling;

	Renderer::ParticleEventQueue* _particleEventQueue;

	void ReleaseGameObjects();

public:
	ObjectHandler(ID3D11Device* device, AssetManager* assetManager, GameObjectInfo* data, System::Settings* settings, Renderer::ParticleEventQueue* particleReque, System::SoundModule*	soundModule);
	~ObjectHandler();

	//Add a gameobject
	bool Add(System::Blueprint* blueprint, int textureId, const XMFLOAT3& position, const XMFLOAT3& rotation, const bool placeOnTilemap = true, const vector<int>& additionalData = vector<int>());
	
	bool Remove(int ID);
	bool Remove(System::Type type, int ID);
	bool Remove(GameObject* gameObject);

	GameObject* Find(int ID);
	GameObject* Find(System::Type type, int ID);
	GameObject* Find(System::Type type, short index);

	//Returns a vector containing all gameobjects with the same type
	vector<GameObject*>* GetAllByType(System::Type type);
	//Returns a list of a renderobject and matrices for all objects using the renderobject
	RenderList GetAllByType(int renderObjectID);
	vector<vector<GameObject*>>* GetGameObjects();

	map<GameObject*, Renderer::Spotlight*>* GetSpotlights();
	map<GameObject*, Renderer::Pointlight*>* GetPointlights();
	vector<vector<GameObject*>>* GetObjectsInLight(Renderer::Spotlight* spotlight);

	int GetObjectCount() const;

	Tilemap* GetTileMap() const;
	void SetTileMap(Tilemap* tilemap);
	void MinimizeTileMap();
	void EnlargeTilemap(int offset);
	Grid* GetBuildingGrid();

	Level::LevelHeader* GetCurrentLevelHeader();
	void SetCurrentLevelHeader(Level::LevelHeader levelheader);
	bool LoadLevel(std::string levelBinaryFilePath);
	void UnloadLevel();

	void InitPathfinding();
	void EnableSpawnPoints();
	void DisableSpawnPoints();
	int GetRemainingToSpawn()const;

	Renderer::ParticleEventQueue* GetParticleEventQueue();

	//Update gamelogic of all objects
	void Update(float deltaTime);
	void UpdateLights();

	vector<System::Blueprint>* GetBlueprints();
	System::Blueprint* GetBlueprintByName(string name);
	System::Blueprint* GetBlueprintByType(int type, int subType = 0);
};

