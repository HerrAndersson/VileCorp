#include "ObjectHandler.h"



ObjectHandler::ObjectHandler(ID3D11Device* device, AssetManager* assetManager, QuadTreeCulling* quadTreeCulling)
{
	_idCount = 0;
	_assetManager = assetManager;
	_quadTreeCulling = quadTreeCulling;
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
	case SPAWN:
		object = new Architecture(_idCount, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(type));
		addedObject = _tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	case ENEMY:
	case GUARD:
		//TODO Tileposition parameters are temporary
		object = new Enemy(_idCount, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(type), _tilemap);
		addedObject = _tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	case TRAP:
		object = new Trap(_idCount, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(type));
		addedObject = _tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	case TRIGGER:
		object = new Trigger(_idCount, position, rotation, AI::Vec2D(1,1), type, _assetManager->GetRenderObject(type));
		addedObject = _tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	default:
		
		break;
	}

	_idCount++;
	
	if (addedObject == true)
	{
		_gameObjects[type].push_back(object);
		_objectCount++;
	}
	else
	{
		//TODO: Log failed add attempts - Rikhard
		delete object;

	}

	return addedObject;
}

bool ObjectHandler::Remove(int ID)
{
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (int j = 0; j < _gameObjects[i].size(); j++)
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
	for (int i = 0; i < _gameObjects[type].size(); i++)
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
	list._renderObject = _assetManager->GetRenderObject(renderObjectID);;

	int count = 0;
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (GameObject* g : _gameObjects[i])
		{
			if (g->GetRenderObject() == _assetManager->GetRenderObject(renderObjectID))
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
		Add((Type)i._tileType, i._tileType, DirectX::XMFLOAT3(i._posX, 0, i._posZ), DirectX::XMFLOAT3(0, i._rotY, 0));
	}
	_quadTreeCulling->CreateTree(_tilemap);

	return false;
}

void ObjectHandler::InitPathfinding()
{
	for (vector<GameObject*>::iterator i = _gameObjects[ENEMY].begin();
	i != _gameObjects[ENEMY].end(); i++)
	{
		if ((*i)->GetType() == ENEMY)																//Handle unit movement
		{
			Unit* unit = dynamic_cast<Unit*>((*i));
			unit->CheckAllTiles();
			unit->Move();
		}
	}
}

void ObjectHandler::Update(float deltaTime)
{
	//Update all objects gamelogic

	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (GameObject* g : _gameObjects[i])
		{
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
					//drop held object and set its tile position
					_tilemap->RemoveObjectFromTile(g->GetTilePosition()._x, g->GetTilePosition()._y, unit);
					unit->Release();
					delete unit;
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
