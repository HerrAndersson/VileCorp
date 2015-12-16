#include "ObjectHandler.h"



ObjectHandler::ObjectHandler(ID3D11Device* device, AssetManager* assetManager)
{
	_idCounter = 0;
	_assetManager = assetManager;
	_tilemap = nullptr;

	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		_gameObjects.push_back(std::vector<GameObject*>());
	}
}

ObjectHandler::~ObjectHandler()
{
	Release();
	delete _tilemap;
}

bool ObjectHandler::Add(Type type, int renderObjectID, XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	GameObject* object = nullptr;
	bool addedObject = false;

	switch (type)
	{
	case FLOOR:
	case WALL:
	case LOOT:
		object = new Architecture(_idCounter++, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(renderObjectID));
		_tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	case UNIT:
		//TODO Tileposition parameters are temporary
		object = new Enemy(_idCounter++, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(renderObjectID), _tilemap);
		_tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	case TRAP:
		object = new Trap(_idCounter++, position, rotation, AI::Vec2D(1,1), type, _assetManager->GetRenderObject(renderObjectID));
		_tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	case TRIGGER:
		object = new Trigger(_idCounter++, position, rotation, AI::Vec2D(1,1), type, _assetManager->GetRenderObject(renderObjectID));
		_tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	default:
		
		break;
	}

	if (addedObject == true)
	{
		_gameObjects[type].push_back(object);
	}

	return addedObject;
}

bool ObjectHandler::Remove(Type type, int ID)
{
	bool removed = false;

	for (vector<GameObject*>::iterator i = _gameObjects[type].begin();
	i != _gameObjects[type].end(); i++)
	{
		if ((*i)->GetID() == ID)
		{
			// Release object resource
			(*i)->Release();

			// Replace pointer with the last pointer int the vector
			(*i) = (*_gameObjects[type].end());

			// Remove pointer value to avoid various problems
			_gameObjects[type].pop_back();

			removed = true;
		}
	}
	return removed;
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

GameObject* ObjectHandler::Find(Type type, short index)
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
	// Har jag missförstått något? /Zache

	RenderList list;
	list._renderObject = _assetManager->GetRenderObject(renderObjectID);;

	int count = 0;
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (GameObject* g : _gameObjects[i])
		{
			if (g->GetRenderObject() == _assetManager->GetRenderObject(renderObjectID))
			{
				list._modelMatrices[count] = g->GetMatrix();
			}
			count++;
		}
	}

	return list;
}

int ObjectHandler::GetTotalNrOfGameObjects() const
{
	int totalSize = 0;
	for (int i = 0; i < _gameObjects.size(); i++)
	{
		totalSize += _gameObjects[i].size();
	}
	return totalSize;
}

vector<vector<GameObject*>>* ObjectHandler::GetGameObjects()
{
	return &_gameObjects;
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
		Add((Type)i._tileType, i._tileType, DirectX::XMFLOAT3(i._posX, 0, i._posZ), DirectX::XMFLOAT3(0, i._rotY, 0));
	}
	return false;
}

void ObjectHandler::InitPathfinding()
{
	for (vector<GameObject*>::iterator i = _gameObjects[UNIT].begin();
	i != _gameObjects[UNIT].end(); i++)
	{
		if ((*i)->GetType() == UNIT)																	//Handle unit movement
		{
			Unit* unit = dynamic_cast<Unit*>((*i));
			unit->CheckAllTiles();
			unit->CalculatePath(unit->GetGoal());
		}
	}
}

void ObjectHandler::Update()
{
	//Update all objects gamelogic

	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (GameObject* g : _gameObjects[i])
		{
			g->Update();
			if (g->GetType() == UNIT)																	//Handle unit movement
			{
				float xOffset = abs(g->GetPosition().x - g->GetTilePosition()._x);
				float zOffset = abs(g->GetPosition().z - g->GetTilePosition()._y);
				if (xOffset > 0.98 || zOffset > 0.98)																//If unit is on a new tile	
				{
					Unit* unit = dynamic_cast<Unit*>(g);
					_tilemap->RemoveObjectFromTile(g->GetTilePosition()._x, g->GetTilePosition()._y, unit);
					unit->Move();
					_tilemap->AddObjectToTile(g->GetTilePosition()._x, g->GetTilePosition()._y, unit);
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
	}
	_idCounter = 0;
}
