#pragma once
#include <vector>
#include "GameObject.h"
#include "Unit.h"
#include "Trap.h"
#include "Architecture.h"
#include "Trigger.h"
#include "Enemy.h"
#include "Guard.h"
#include "Tilemap.h"
#include "JsonStructs.h"
#include "AssetManager.h"

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
	//vector<RenderList> _renderLists;
	GameObjectInfo* _gameObjectData;
	void ActivateTileset(string name);
	Tilemap* _tilemap;

	int _idCount = 0;
	int _objectCount = 0;

	AssetManager* _assetManager;

public:
	ObjectHandler(ID3D11Device* device, AssetManager* assetManager, GameObjectInfo* data);
	~ObjectHandler();

	//Add a gameobject
	bool Add(Type type, XMFLOAT3 position, XMFLOAT3 rotation);
	
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

	int GetObjectCount() const;

	Tilemap* GetTileMap() const;
	void SetTileMap(Tilemap* tilemap);

	bool LoadLevel(int lvlIndex);

	void InitPathfinding();

	//Update gamelogic of all objects
	void Update(float deltaTime);
	//Relase all object resources
	void Release();
};

