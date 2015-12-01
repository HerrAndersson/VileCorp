#include "ObjectHandler.h"



ObjectHandler::ObjectHandler(ID3D11Device* device)
{
	_size = 0;
	_assetManager = new AssetManager(device);
}

ObjectHandler::~ObjectHandler()
{
	delete _assetManager;
	delete _tilemap;
}


int ObjectHandler::GetSize() const
{
	return _size;
}

GameObject* ObjectHandler::Add(Type type, int renderObjectID, XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	GameObject* object = nullptr;

	switch (type)
	{
	case UNIT:
		_gameObjects.push_back((GameObject*)new Unit(_idCounter++, position, renderObjectID));
		_size++;
		break;
	case STRUCTURE:
		break;
	case TILE:
		break;
	case TRAP:
		break;
	case TRIGGER:
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
		if (_gameObjects.at(i)->GetRenderObjectID() == renderObjectID)
		{
			list.modelMatrices.push_back(_gameObjects[i]->GetModelMatrix());
		}
	}

	return list;
}

//std::vector<RenderObject*> ObjectHandler::GetRenderObjects() const
//{
//	std::vector<RenderObject*> renderObjects;
//
//	for (int i = 0; i < _size; i++)
//	{
//		//If the object has a Renderobject and is visible
//		if (_gameObjects[i]->GetRenderObject() != nullptr && _gameObjects[i]->IsVisible())
//		{
//			renderObjects.push_back(_gameObjects[i]->GetRenderObject());
//		}
//	}
//
//	return renderObjects;
//}

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
		//_renderModule->Render(/*_gameObjects[i]->GetRenderObjectID(), _gameObjects[i]->GetModelMatrix()*/);
	}
}

void ObjectHandler::Release()
{
	for (int i = 0; i < _size; i++)
	{
		_gameObjects[i]->Release();
		delete _gameObjects[i];
	}

}
