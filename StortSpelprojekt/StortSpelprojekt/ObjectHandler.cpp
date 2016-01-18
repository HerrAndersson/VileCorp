#include "ObjectHandler.h"



ObjectHandler::ObjectHandler(ID3D11Device* device, AssetManager* assetManager)
{
	_size = 0;
	_idCounter = 0;
	_assetManager = assetManager;
	_tilemap = nullptr;
}

ObjectHandler::~ObjectHandler()
{
	delete _tilemap;
	Release();
}


int ObjectHandler::GetSize() const
{
	return _size;
}

GameObject* ObjectHandler::Add(Type type, int renderObjectID, XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	GameObject* object = nullptr;

	switch (type)
	{
	case FLOOR:
	case WALL:
	case LOOT:
		object = new Architecture(_idCounter++, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(renderObjectID));
		_gameObjects.push_back(object);
		_size++;
		_tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	case ENEMY:
	case GUARD:
		//Temporary. Enemies and guards should replace units properly
		object = new Enemy(_idCounter++, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(renderObjectID), _tilemap);
		_gameObjects.push_back(object);
		_size++;
		_tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	case TRAP:
		object = new Trap(_idCounter++, position, rotation, AI::Vec2D(1,1), type, _assetManager->GetRenderObject(renderObjectID));
		_gameObjects.push_back(object);
		_size++;
		_tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	case TRIGGER:
		object = new Trigger(_idCounter++, position, rotation, AI::Vec2D(1,1), type, _assetManager->GetRenderObject(renderObjectID));
		_gameObjects.push_back(object);
		_size++;
		_tilemap->AddObjectToTile((int)position.x, (int)position.z, object);
		break;
	default:
		
		break;
	}

	return object;
}

bool ObjectHandler::Remove(short ID)
{
	bool removed = false;

	for (int i = 0; i < _size && !removed; i++)
	{
		if (_gameObjects[i]->GetID() == ID)
		{
			// Release object resource
			_gameObjects[i]->Release();

			// Decrease size of vector
			_size--;

			// Replace pointer with the last pointer int the vector
			_gameObjects[i] = _gameObjects[_size];

			// Remove pointer value to avoid various problems
			_gameObjects.pop_back();
			
			removed = true;
		}
	}

	return removed;
}

void ObjectHandler::Clear()
{
	_gameObjects.clear();
	_idCounter = 0;
	_size = 0;
}

GameObject* ObjectHandler::Find(short ID)
{
	GameObject* gameObject = nullptr;
	bool found = false;

	for (int i = 0; i < _size && !found; i++)
	{
		if (_gameObjects[i]->GetID() == ID)
		{
			gameObject = _gameObjects[i];
			found = true;
		}
	}

	return gameObject;
}

GameObject* ObjectHandler::Find(int index)
{
	GameObject* gameObject = nullptr;
	if (index >= 0 && index < _size)
	{
		gameObject = _gameObjects[index];
	}
	
	return gameObject;
}

vector<GameObject*> ObjectHandler::GetAll(Type type)
{
	vector<GameObject*> list;
	for (int i = 0; i < _size; i++)
	{
		if (_gameObjects.at(i)->GetType() == type)
		{
			list.push_back(_gameObjects[i]);
		}
	}

	return list;
}//TODO make it so that it returns a vector of renderlists to improve performance /Markus
RenderList ObjectHandler::GetAll(int renderObjectID)
{
	RenderList list;

	list._renderObject = _assetManager->GetRenderObject(renderObjectID);
	list._modelMatrices.resize(_size);
	for (int i = 0; i < _size; i++)
	{
		if (_gameObjects.at(i)->GetRenderObject() == _assetManager->GetRenderObject(renderObjectID))
		{	
			list._modelMatrices[i] = _gameObjects[i]->GetMatrix();
		}
	}

	return list;
}

std::vector<GameObject*>* ObjectHandler::GetGameObjects()
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

	Clear();

	delete _tilemap;
	_tilemap = new Tilemap(dimX, dimY);

	Type tileType = FLOOR;
	for (auto i : gameObjectData)
	{
		switch (i._tileType)
		{
		case 0:
			tileType = ENEMY;
			break;
		case 1:
			tileType = FLOOR;
			break;
		case 2:
			tileType = WALL;
			break;
		case 3:
			tileType = LOOT;
			break;
		default:
			break;
		}
		Add(tileType, tileType, DirectX::XMFLOAT3(i._posX, 0, i._posZ), DirectX::XMFLOAT3(0, i._rotY, 0));
	}
	return false;
}

void ObjectHandler::InitPathfinding()
{
	for (int i = 0; i < _size; i++)
	{
		if (_gameObjects[i]->GetType() == GUARD || _gameObjects[i]->GetType() == ENEMY)												//Initiate unit movement
		{
			Unit* unit = dynamic_cast<Unit*>(_gameObjects[i]);
			unit->CheckAllTiles();
			//unit->CalculatePath(unit->GetGoal());
			unit->Move();
		}
	}
}

void ObjectHandler::Update()
{
	//Update all objects gamelogic
	for (int i = 0; i < _size; i++)
	{
		_gameObjects[i]->Update();

		if (_gameObjects[i]->GetPickUpState() == PICKINGUP)
		{
			_tilemap->RemoveObjectFromTile(_gameObjects[i]->GetTilePosition()._x, _gameObjects[i]->GetTilePosition()._y, _gameObjects[i]);
			_gameObjects[i]->SetPickUpState(HELD);
		}
		else if (_gameObjects[i]->GetPickUpState() == DROPPING)
		{
			_tilemap->AddObjectToTile(_gameObjects[i]->GetTilePosition()._x, _gameObjects[i]->GetTilePosition()._y, _gameObjects[i]);
			_gameObjects[i]->SetPickUpState(ONTILE);
		}

		if (_gameObjects[i]->GetType() == GUARD || _gameObjects[i]->GetType() == ENEMY)									//Handle unit movement
		{
			Unit* unit = dynamic_cast<Unit*>(_gameObjects[i]);

			GameObject* heldObject = unit->GetHeldObject();

			if (heldObject != nullptr)
			{
				heldObject->SetPosition(unit->GetPosition());
			}
			if (unit->GetHealth() <= 0)
			{
				//drop held object and set its tile position
				_tilemap->RemoveObjectFromTile(_gameObjects[i]->GetTilePosition()._x, _gameObjects[i]->GetTilePosition()._y, unit);
				unit->Release();
				delete unit;
			}
			else
			{
				float xOffset = abs(_gameObjects[i]->GetPosition().x - _gameObjects[i]->GetTilePosition()._x);
				float zOffset = abs(_gameObjects[i]->GetPosition().z - _gameObjects[i]->GetTilePosition()._y);
				if (xOffset > 0.99 || zOffset > 0.99)																		 //If unit is on a new tile	
				{
					_tilemap->RemoveObjectFromTile(_gameObjects[i]->GetTilePosition()._x, _gameObjects[i]->GetTilePosition()._y, unit);
					unit->Move();
					_tilemap->AddObjectToTile(_gameObjects[i]->GetTilePosition()._x, _gameObjects[i]->GetTilePosition()._y, unit);
				}
			}
		}
	}
}

void ObjectHandler::Release()
{
	//_gameObjects[72]->Release();
	//delete _gameObjects[72];
	for (int i = 0; i < _size; i++)
	{
		_tilemap->RemoveObjectFromTile(_gameObjects[i]->GetTilePosition()._x, _gameObjects[i]->GetTilePosition()._y, _gameObjects[i]);
		_gameObjects[i]->Release();
		delete _gameObjects[i];
	}
}
