#include "ObjectHandler.h"

ObjectHandler::ObjectHandler(ID3D11Device* device, AssetManager* assetManager, GameObjectInfo* data)
{
	_idCount = 0;
	_assetManager = assetManager;
	_tilemap = nullptr;
	_gameObjectInfo = data;

	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		_gameObjects.push_back(std::vector<GameObject*>());
	}
	ActivateTileset("default2");
}

ObjectHandler::~ObjectHandler()
{
	Release();
	delete _tilemap;
	delete _gameObjectInfo;
}

void ObjectHandler::ActivateTileset(string name)
{
	Release();
	_assetManager->ActivateTileset(name);

	for (uint a = 0; a < Type::NR_OF_TYPES; a++)
	{
		for (uint i = 0; i < _gameObjectInfo->_objects[a]->size(); i++)
		{
			_gameObjectInfo->_objects[a]->at(i)->_renderObject = _assetManager->GetRenderObjectByType((Type)a, i);
		}
	}
}



bool ObjectHandler::Add(Type type, int index, XMFLOAT3 position, XMFLOAT3 rotation)
{
	GameObject* object = nullptr;

	switch (type)
	{
	case FLOOR:
		object = MakeFloor(_gameObjectInfo->Floors(index), position, rotation);
		break;
	case WALL:
		object = MakeWall(_gameObjectInfo->Walls(index), position, rotation);
		break;
	case LOOT:
		object = MakeLoot(_gameObjectInfo->Loot(index), position, rotation);
		break;
	case SPAWN:
		object = MakeSpawn(_gameObjectInfo->Spawns(index), position, rotation);
		break;
	case ENEMY:
		object = MakeEnemy(_gameObjectInfo->Enemies(index), position, rotation);
		break;
	case GUARD:
		object = MakeGuard(_gameObjectInfo->Guards(index), position, rotation);
		break;
	case TRAP:
		object = MakeTrap(_gameObjectInfo->Traps(index), position, rotation);
		break;
	case TRIGGER:
		object = MakeTrigger(_gameObjectInfo->Triggers(index), position, rotation);
		break;
	default:

		break;
	}


	if (object != nullptr)
	{
		_idCount++;
		_gameObjects[type].push_back(object);
		_objectCount++;
		return true;
	}

	return false;
}

bool ObjectHandler::Add(Type type, std::string name, XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	for (int i = 0; i < _gameObjectInfo->_objects[type]->size(); i++)
	{
		if (_gameObjectInfo->_objects[type]->at(i)->_name == name)
		{
			return Add(type, i, position, rotation);
		}
	}
	return false;
}

bool ObjectHandler::Remove(int ID)
{
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (uint j = 0; j < _gameObjects[i].size(); j++)
		{
			if (_gameObjects[i][j]->GetID() == ID)
			{
				// Release object resource
				_gameObjects[i][j]->Release();

				delete _gameObjects[i][j];

				// Replace pointer with the last pointer int the vector
				_gameObjects[i][j] = _gameObjects[i].back();

				// Remove pointer value to avoid various problems
				_gameObjects[i].pop_back();

				_objectCount--;

				return true;
			}
		}
	}


	//for (vector<GameObject*> v : _gameObjects)
	//{
	//	for (int i = 0; i < v.size(); i++)
	//	{
	//		if (v[i]->GetID() == ID)
	//		{
	//			// Release object resource
	//			v[i]->Release();

	//			delete v[i];

	//			// Replace pointer with the last pointer int the vector
	//			v[i] = v.back();

	//			// Remove pointer value to avoid various problems
	//			v.pop_back();

	//			_objectCount--;

	//			return true;
	//		}
	//	}
	//}
	return false;
}

bool ObjectHandler::Remove(Type type, int ID)
{
	for (uint i = 0; i < _gameObjects[type].size(); i++)
	{
		if (_gameObjects[type][i]->GetID() == ID)
		{
			_tilemap->RemoveObjectFromTile(_gameObjects[type].at(i));
			_gameObjects[type][i]->Release();

			delete _gameObjects[type][i];

			// Replace pointer with the last pointer int the vector
			_gameObjects[type][i] = _gameObjects[type].back();

			// Remove pointer value to avoid various problems
			_gameObjects[type].pop_back();

			_objectCount--;

			return true;
		}
	}

	return false;
}

GameObject * ObjectHandler::Find(int ID)
{
	for (vector<GameObject*> v : _gameObjects)
	{
		for (GameObject* g : v)
		{
			if (g->GetID() == ID)
			{
				return g;
			}
		}
	}
	return nullptr;
}

bool ObjectHandler::Remove(GameObject* gameObject)
{
	return Remove(gameObject->GetType(), gameObject->GetID());
}

GameObject* ObjectHandler::Find(Type type, int ID)
{
	for (GameObject* g : _gameObjects[type])
	{
		if (g->GetID() == ID)
		{
			return g;
		}
	}
	return nullptr;
}

GameObject* ObjectHandler::Find(Type type, short index)//TODO why? - Fredrik
{
	for (GameObject* g : _gameObjects[type])
	{
		return g;
	}
	return nullptr;
}

vector<GameObject*> ObjectHandler::GetAllByType(Type type)
{
	return _gameObjects[type];
}

RenderList ObjectHandler::GetAllByType(int renderObjectID)
{
	// TODO make it so that it returns a vector of renderlists to improve performance /Markus

	RenderList list;
	list._renderObject = _assetManager->GetRenderObject(renderObjectID, "");;

	int count = 0;
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (GameObject* g : _gameObjects[i])
		{
			if (g->GetRenderObject() == _assetManager->GetRenderObject(renderObjectID, ""))
			{
				//TODO _modelMatrices should hold XMMATRIX* instead to ensure proper alignment of the matrices |Jonas a.k.a. ] 0 |\| /-\ 5
				list._modelMatrices[count] = *g->GetMatrix();
			}
			count++;
		}
	}

	return list;
}

vector<vector<GameObject*>>* ObjectHandler::GetGameObjects()
{
	return &_gameObjects;
}

GameObjectInfo * ObjectHandler::GetBlueprints()
{
	return _gameObjectInfo;
}

int ObjectHandler::GetObjectCount() const
{
	return _objectCount;
}

Tilemap * ObjectHandler::GetTileMap() const
{
	return _tilemap;
}

void ObjectHandler::SetTileMap(Tilemap * tilemap)
{
	_tilemap = tilemap;
}

bool ObjectHandler::LoadLevel(int lvlIndex)
{
	int dimX, dimY;
	vector<GameObjectData> gameObjectData;
	_assetManager->ParseLevel(lvlIndex, gameObjectData, dimX, dimY);

	delete _tilemap;
	_tilemap = new Tilemap(dimX, dimY);

	for (auto i : gameObjectData)
	{
		Add((Type)i._tileType, 0, DirectX::XMFLOAT3(i._posX, 0, i._posZ), DirectX::XMFLOAT3(0, i._rotY, 0));
	}
	return false;
}

void ObjectHandler::InitPathfinding()
{
	for (vector<GameObject*>::iterator i = _gameObjects[ENEMY].begin(); i != _gameObjects[ENEMY].end(); i++)
	{
		Unit* unit = dynamic_cast<Unit*>((*i));
		//unit->CheckAllTiles();
		unit->Move();
	}
	for (vector<GameObject*>::iterator i = _gameObjects[GUARD].begin(); i != _gameObjects[GUARD].end(); i++)
	{

		Unit* unit = dynamic_cast<Unit*>((*i));
		//unit->CheckAllTiles();
		unit->Move();
	}
}

void ObjectHandler::Update(float deltaTime)
{
	//Update all objects gamelogic

	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (int j = 0; j < _gameObjects[i].size(); j++)
		{
			GameObject* g = _gameObjects[i][j];
			g->Update();

			if (g->GetPickUpState() == PICKINGUP)
			{
				_tilemap->RemoveObjectFromTile(g);
				g->SetPickUpState(HELD);
			}
			else if (g->GetPickUpState() == DROPPING)
			{
				_tilemap->AddObjectToTile(g->GetTilePosition()._x, g->GetTilePosition()._y, g);
				g->SetPickUpState(ONTILE);
			}

			if (g->GetType() == GUARD || g->GetType() == ENEMY)									//Handle unit movement
			{
				Unit* unit = dynamic_cast<Unit*>(g);

				GameObject* heldObject = unit->GetHeldObject();

				if (heldObject != nullptr)
				{
					heldObject->SetPosition(DirectX::XMFLOAT3(unit->GetPosition().x, unit->GetPosition().y + 2, unit->GetPosition().z));
				}
				if (unit->GetHealth() <= 0)
				{
					//TODO: drop held object and set its tile position --Victor
					Remove(g);
					j--;
				}
				else
				{
					float xOffset = abs(g->GetPosition().x - g->GetTilePosition()._x);
					float zOffset = abs(g->GetPosition().z - g->GetTilePosition()._y);
					if (xOffset > 0.99 || zOffset > 0.99)																		 //If unit is on a new tile	
					{
						_tilemap->RemoveObjectFromTile(g->GetTilePosition()._x, g->GetTilePosition()._y, unit);
						unit->Move();
						_tilemap->AddObjectToTile(g->GetTilePosition()._x, g->GetTilePosition()._y, unit);
					}
				}
			}
		}
	}
}

void ObjectHandler::Release()
{
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (GameObject* g : _gameObjects[i])
		{
			g->Release();
			delete g;
		}
		_gameObjects[i].clear();
	}
	_idCount = 0;
	_objectCount = 0;
}

Architecture * ObjectHandler::MakeFloor(GameObjectFloorInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
{
	Architecture* obj = new Architecture(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		FLOOR,
		_assetManager->GetRenderObject(data->_renderObject, data->_textureName));

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Architecture * ObjectHandler::MakeWall(GameObjectWallInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
{
	Architecture* obj = new Architecture(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		WALL,
		_assetManager->GetRenderObject(data->_renderObject, data->_textureName));

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Architecture * ObjectHandler::MakeLoot(GameObjectLootInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
{
	Architecture* obj = new Architecture(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		LOOT,
		_assetManager->GetRenderObject(data->_renderObject, data->_textureName));

	// read more data

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Architecture * ObjectHandler::MakeSpawn(GameObjectSpawnInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
{
	Architecture* obj = new Architecture(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		SPAWN,
		_assetManager->GetRenderObject(data->_renderObject, ""));

	// read more data

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Trap * ObjectHandler::MakeTrap(GameObjectTrapInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
{
	Trap* obj = new Trap(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		TRAP,
		_assetManager->GetRenderObject(data->_renderObject, ""),
		data->_cost);

	// Read more data

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Trigger * ObjectHandler::MakeTrigger(GameObjectTriggerInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
{
	Trigger* obj = new Trigger(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		TRIGGER,
		_assetManager->GetRenderObject(data->_renderObject, ""));

	// read more data

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Guard * ObjectHandler::MakeGuard(GameObjectGuardInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
{
	Guard* obj = new Guard(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		GUARD,
		_assetManager->GetRenderObject(data->_renderObject, data->_textureName),
		_tilemap);

	// read more data

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Enemy * ObjectHandler::MakeEnemy(GameObjectEnemyInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
{
	Enemy* obj = new Enemy(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		ENEMY,
		_assetManager->GetRenderObject(data->_renderObject, data->_textureName),
		_tilemap);

	// read more data

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}
