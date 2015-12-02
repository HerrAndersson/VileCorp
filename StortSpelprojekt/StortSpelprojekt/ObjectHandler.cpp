#include "ObjectHandler.h"



ObjectHandler::ObjectHandler()
{
	_size = 0;
	_idCounter = 0;
	_assetManager = nullptr;
	_tilemap = nullptr;
}

ObjectHandler::ObjectHandler(ID3D11Device* device)
{
	_size = 0;
	_idCounter = 0;
	_assetManager = new AssetManager(device);
	_tilemap = nullptr;
}

ObjectHandler::~ObjectHandler()
{
	Release();
	Clear();
	delete _assetManager;
	delete _tilemap;
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
	case UNIT:
		_gameObjects.push_back((GameObject*)new Unit(_idCounter++, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(renderObjectID), _tilemap));
		_size++;
		break;
	case TRAP:
		_gameObjects.push_back((GameObject*)new Trap(_idCounter++, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(renderObjectID)));
		_size++;
		break;
	case TRIGGER:
		_gameObjects.push_back((GameObject*)new Trigger(_idCounter++, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(renderObjectID)));
		_size++;
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

	return list;;
}

RenderList ObjectHandler::GetAll(int renderObjectID)
{
	RenderList list;

	list._renderObject = _assetManager->GetRenderObject(renderObjectID);
	for (int i = 0; i < _size; i++)
	{
		if (_gameObjects.at(i)->GetRenderObject() == _assetManager->GetRenderObject(renderObjectID))
		{
			list.modelMatrices.push_back(_gameObjects[i]->GetMatrix());
		}
	}

	return list;
}

Tilemap * ObjectHandler::GetTileMap() const
{
	return _tilemap;
}

void ObjectHandler::SetTileMap(Tilemap * tilemap)
{
	_tilemap = tilemap;
}

bool ObjectHandler::LoadLevel(string filename)
{
	return false;
}

void ObjectHandler::Update()
{
	//Update all objects gamelogic
	for (int i = 0; i < _size; i++)
	{
		_gameObjects[i]->Update();
	}
}

void ObjectHandler::Release()
{
	for (int i = 0; i < _size; i++)
	{
		_gameObjects[i]->Release();
	}
}
