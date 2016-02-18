#include "ObjectHandler.h"
#include "stdafx.h"

ObjectHandler::ObjectHandler(ID3D11Device* device, AssetManager* assetManager, GameObjectInfo* data, System::Settings* settings)
{
	_settings = settings;
	_idCount = 0;
	_assetManager = assetManager;
	_tilemap = new Tilemap();
	_buildingGrid = new Grid(device, 1, 1, 1, XMFLOAT3(1.0f, 1.0f, 0.7f));
	_gameObjectInfo = data;
	_device = device;
	_lightCulling = nullptr;
	_gameObjects.resize(NR_OF_TYPES);
}

ObjectHandler::~ObjectHandler()
{
	UnloadLevel();
	SAFE_DELETE(_buildingGrid);
}

bool ObjectHandler::LoadLevel(std::string levelBinaryFilePath)
{
	bool result = false;
	LevelBinary levelData;
	HRESULT success = _assetManager->ParseLevelBinary(&levelData, levelBinaryFilePath);

	if (success == S_OK)
	{
		result = true;
		delete _tilemap;
		_tilemap = new Tilemap(AI::Vec2D(levelData._tileMapSizeX, levelData._tileMapSizeZ));

		for (int i = 0; i < levelData._gameObjectData.size() && result; i++)
		{
			float rotation;
			if (levelData._gameObjectData[i][2] == 3)
			{
				rotation = 0;
			}
			else if (levelData._gameObjectData[i][2] == 0)
			{
				rotation = DirectX::XM_PIDIV2;
			}
			else if (levelData._gameObjectData[i][2] == 1)
			{
				rotation = DirectX::XM_PI;
			}
			else
			{
				rotation = DirectX::XM_PI + DirectX::XM_PIDIV2;
			}
			//result = Add(DirectX::XMFLOAT3(
			//	levelData->_gameObjectData[i][0], 0, levelData->_gameObjectData[i][1]),
			//	DirectX::XMFLOAT3(0, rotation, 0),
			//	Type(levelData->_gameObjectData[i][3]),
			//	levelData->_gameObjectData[i][4],
			//	levelData->_gameObjectData[i][5],
			//	levelData->_gameObjectData[i][6]);
		}
	}
	else
	{
		_tilemap = new Tilemap();
	}
	return result;
}

void ObjectHandler::UnloadLevel()
{
	for (pair<GameObject*, Renderer::Spotlight*> spot : _spotlights)
	{
		SAFE_DELETE(spot.second);
		spot.second = nullptr;
		spot.first = nullptr;
	}
	_spotlights.clear();
	for (pair<GameObject*, Renderer::Pointlight*> point : _pointligths)
	{
		SAFE_DELETE(point.second);
		point.second = nullptr;
		point.first = nullptr;
	}
	_pointligths.clear();

	ReleaseGameObjects();
	SAFE_DELETE(_tilemap);
	SAFE_DELETE(_lightCulling);
}

bool ObjectHandler::Add(Blueprint* blueprint, int textureId, const XMFLOAT3& position, const XMFLOAT3& rotation, const bool placeOnTilemap)
{
	GameObject* object = nullptr;
	Type type = (Type)blueprint->_type;
	RenderObject* renderObject = _assetManager->GetRenderObject(blueprint->_mesh, blueprint->_textures[textureId]);

	AI::Vec2D tilepos(position.x, position.z);

	switch (type)
	{
	case WALL:
	case FLOOR:
	case FURNITURE:
	case LOOT:
		object = new Architecture(_idCount, position, rotation, tilepos, type, renderObject);
		break;
	case SPAWN:
		object = new SpawnPoint(_idCount, position, rotation, tilepos, type, renderObject, 66, 6);
		break;
	case TRAP:
		object = new Trap(_idCount, position, rotation, tilepos, type, renderObject, _tilemap, blueprint->_subType);
		break;
	case CAMERA:
		object = new SecurityCamera(_idCount, position, rotation, tilepos, type, renderObject, _tilemap);
		break;
	case ENEMY:
		object = new Enemy(_idCount, position, rotation, tilepos, type, renderObject, _tilemap);
		break;
	case GUARD:
		object = new Guard(_idCount, position, rotation, tilepos, type, renderObject, _tilemap);
		break;
	default:
		break;
	}

	if (placeOnTilemap)
	{
		if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, object))
		{
			delete object;
			object = nullptr;
		}
	}

	//Temporary check for traps. Could be more general if other objects are allowed to take up multiple tiles
	bool addedObject = false;
	if (object != nullptr)
	{
		addedObject = true;
	}
	if (type == TRAP && addedObject && !placeOnTilemap)
	{
		Trap* trap = static_cast<Trap*>(object);
		int i = 0;
		AI::Vec2D* arr = trap->GetTiles();
		for (int i = 0; i < trap->GetTileSize() && addedObject; i++)
		{
			if (!_tilemap->CanPlaceObject(arr[i]))
			{
				addedObject = false;
			}
		}
		if (addedObject)
		{
			for (int i = 0; i < trap->GetTileSize() && addedObject; i++)
			{
				_tilemap->AddObjectToTile(arr[i], object);
			//	_tilemap->GetObjectOnTile(arr[i], FLOOR)->AddColorOffset({2,0,0});
			}
		}

	}

	if (object != nullptr)
	{
		_idCount++;
		_gameObjects[type].push_back(object);

		//TODO: remove when proper loading can be done /Jonas
		if (type == GUARD)
		{
			SpotlightData d;
			d._angle = 0.6f * XM_PI;
			d._bone = -1;
			d._color = XMFLOAT3(0.9f, 0.9f, 0.9f);
			d._direction = XMFLOAT3(1, 0, 1);
			d._intensity = 1.0f;
			d._pos = XMFLOAT3(0, 0, 0);
			d._range = (float)static_cast<Unit*>(object)->GetVisionRadius();
			_spotlights[object] = new Renderer::Spotlight(_device, d, 0.1f, 1000.0f);
		}
		if (type == CAMERA)
		{
			SpotlightData d;
			d._angle = 0.6f * XM_PI;
			d._bone = -1;
			d._color = XMFLOAT3(0.9f, 0.6f, 0.6f);
			d._direction = XMFLOAT3(1, 0, 1);
			d._intensity = 1.0f;
			d._pos = XMFLOAT3(0, 0, 0);
			d._range = (float)static_cast<SecurityCamera*>(object)->GetVisionRadius();
			_spotlights[object] = new Renderer::Spotlight(_device, d, 0.1f, 1000.0f);
		}
		if (type == LOOT)
		{
			PointlightData d;
			d._pos = XMFLOAT3(object->GetPosition().x, 2, object->GetPosition().z);
			d._range = 6.0f;
			d._intensity = 0.8f;
			d._col = XMFLOAT3(0.9f, 0.5f, 0.5f);
			_pointligths[object] = new Renderer::Pointlight(_device, d._pos, d._range, d._intensity, d._col);
		}

		//for(auto i : object->GetRenderObject()->_mesh._spotLights)
		//{
		//	_spotlights[object] = new Renderer::Spotlight(_device, i, 0.1f, 1000.0f);
		//}
		_objectCount++;
		return true;
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

				if (_spotlights.count(_gameObjects[i][j]))
				{
					delete _spotlights[_gameObjects[i][j]];
					_spotlights.erase(_gameObjects[i][j]);
				}

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

			if (_spotlights.count(_gameObjects[type][i]))
			{
				delete _spotlights[_gameObjects[type][i]];
				_spotlights.erase(_gameObjects[type][i]);
			}

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

bool ObjectHandler::Remove(GameObject* gameObject)
{
	if (gameObject == nullptr)
	{
		return false;
	}
	return Remove(gameObject->GetType(), gameObject->GetID());
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

	RenderList list;
	list._renderObject = _assetManager->GetRenderObject(renderObjectID);

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
	if (_tilemap != nullptr)
	{
		delete _tilemap;
	}
	_tilemap = tilemap;
}

void ObjectHandler::MinimizeTileMap()
{
	if (_tilemap)
	{
		int minY = -1, maxY = -1;
		for (int y = 0; y < _tilemap->GetHeight(); y++)
		{
			for (int x = 0; x < _tilemap->GetWidth() && minY == -1; x++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					minY = y;
				}
			}
		}
		for (int y = _tilemap->GetHeight() - 1; y >= minY; y--)
		{
			for (int x = 0; x < _tilemap->GetWidth() && maxY == -1; x++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					maxY = y + 1;
				}
			}
		}

		int minX = -1, maxX = -1;
		for (int x = 0; x < _tilemap->GetWidth(); x++)
		{
			for (int y = minY; y < maxY && minX == -1; y++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					minX = x;
				}
			}
		}

		for (int x = _tilemap->GetWidth() - 1; x >= minX; x--)
		{
			for (int y = minY; y < maxY && maxX == -1; y++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					maxX = x + 1;
				}
			}
		}

		int newXMax = maxX - minX;
		int newYMax = maxY - minY;

		Tilemap* minimized = new Tilemap(AI::Vec2D(newXMax, newYMax));
		for (int x = 0; x < newXMax; x++)
		{
			for (int y = 0; y < newYMax; y++)
			{
				AI::Vec2D pos;
				pos._x = x + minX;
				pos._y = y + minY;

				std::vector<GameObject*> temp = _tilemap->GetAllObjectsOnTile(pos);

				for (GameObject* g : temp)
				{
					// Update real pos
					g->SetPosition(XMFLOAT3(x, g->GetPosition().y, y));

					// Update tile
					minimized->AddObjectToTile(x, y, g);
				}
			}
		}

		delete _tilemap;
		_tilemap = minimized;

		_buildingGrid->ChangeGridSize(_tilemap->GetWidth(), _tilemap->GetHeight(), 1);
	}
	else
	{
		throw std::runtime_error("ObjectHandler::MinimizeTileMap: _tilemap is nullptr");
	}
}

void ObjectHandler::EnlargeTilemap(int offset)
{
	if (!_tilemap)
	{
		_tilemap = new Tilemap();
	}
	if (offset > 0)
	{
		int o = 2 * offset;
		Tilemap* large = new Tilemap(AI::Vec2D(_tilemap->GetWidth() + o, _tilemap->GetHeight() + o));

		for (int x = offset; x < _tilemap->GetWidth() + offset; x++)
		{
			for (int y = offset; y < _tilemap->GetHeight() + offset; y++)
			{
				AI::Vec2D pos;
				pos._x = x - offset;
				pos._y = y - offset;
				std::vector<GameObject*> temp = _tilemap->GetAllObjectsOnTile(pos);

				for (GameObject* g : temp)
				{
					// Update real pos
					g->SetPosition(XMFLOAT3(x, g->GetPosition().y, y));

					// Update tile
					large->AddObjectToTile(x, y, g);
				}
			}
		}

		delete _tilemap;
		_tilemap = large;

		_buildingGrid->ChangeGridSize(large->GetWidth(), large->GetHeight(), 1);
	}
}

Grid * ObjectHandler::GetBuildingGrid()
{
	return _buildingGrid;
}

void ObjectHandler::InitPathfinding()
{
	for (GameObject* i : _gameObjects[ENEMY])
	{
		Unit* unit = dynamic_cast<Unit*>(i);
		//unit->CheckAllTiles();
		unit->Move();
	}

	for (GameObject* i : _gameObjects[GUARD])
	{
		Unit* unit = dynamic_cast<Unit*>(i);
		//unit->CheckAllTiles();
		unit->Move();
	}
}

void ObjectHandler::EnableSpawnPoints()
{
	for (GameObject* g : _gameObjects[SPAWN])
	{
		((SpawnPoint*)g)->Enable();
	}
}

void ObjectHandler::DisableSpawnPoints()
{
	for (GameObject* g : _gameObjects[SPAWN])
	{
		((SpawnPoint*)g)->Disable();
	}
}

int ObjectHandler::GetRemainingToSpawn() const
{
	int result = 0;
	for (GameObject* g : _gameObjects[3])
	{
		result += static_cast<SpawnPoint*>(g)->GetUnitsToSpawn();
	}
	return result;
}

void ObjectHandler::Update(float deltaTime)
{
	//Update all objects' gamelogic

	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (unsigned int j = 0; j < _gameObjects[i].size(); j++)
		{
			GameObject* g = _gameObjects[i][j];
			g->Update(deltaTime);

			if (g->GetPickUpState() == PICKINGUP)
			{
				_tilemap->RemoveObjectFromTile(g);
				g->SetPickUpState(HELD);
			}
			else if (g->GetPickUpState() == DROPPING)
			{
				_tilemap->AddObjectToTile(g->GetTilePosition(), g);
				g->SetPickUpState(ONTILE);
			}

			if (g->GetType() == GUARD || g->GetType() == ENEMY)									//Handle unit movement
			{
				Unit* unit = static_cast<Unit*>(g);
				GameObject* heldObject = unit->GetHeldObject();

				if (heldObject != nullptr)
				{
					heldObject->SetPosition(DirectX::XMFLOAT3(unit->GetPosition().x, unit->GetPosition().y + 2, unit->GetPosition().z));
					heldObject->SetTilePosition(AI::Vec2D(heldObject->GetPosition().x, heldObject->GetPosition().z));
				}

				if (unit->GetHealth() <= 0 && unit->GetAnimisFinished())
				{
					if (heldObject != nullptr)
					{
						bool lootRemoved = false;

						for (uint k = 0; k < _gameObjects[SPAWN].size() && !lootRemoved; k++)
						{
							//If the enemy is at the despawn point with an objective, remove the objective and the enemy, Aron
							if (_gameObjects[SPAWN][k]->InRange(unit->GetTilePosition()))
							{
								lootRemoved = Remove(heldObject);
								((SpawnPoint*)_gameObjects[SPAWN][k])->AddUnitsToSpawn(1);
								((SpawnPoint*)_gameObjects[SPAWN][k])->Enable();
							}
						}

						if (!lootRemoved)
						{
							heldObject->SetPickUpState(DROPPING);
							heldObject->SetPosition(XMFLOAT3(heldObject->GetPosition().x, 0.0f, heldObject->GetPosition().z));
						}
					}

					Remove(g);
					g = nullptr;
					j--;
				}
				else
				{
					float xOffset = abs(g->GetPosition().x - g->GetTilePosition()._x);
					float zOffset = abs(g->GetPosition().z - g->GetTilePosition()._y);
					if (xOffset > 0.99f || zOffset > 0.99f)																		 //If unit is on a new tile	
					{
						_tilemap->RemoveObjectFromTile(g->GetTilePosition(), unit);
						unit->Move();
						_tilemap->AddObjectToTile(g->GetTilePosition(), unit);

						/*if (_tilemap->IsTrapOnTile(g->GetTilePosition()._x, g->GetTilePosition()._y))
						{
							static_cast<Trap*>(_tilemap->GetObjectOnTile(g->GetTilePosition()._x, g->GetTilePosition()._y, TRAP))->Activate(unit);
						}*/
					}

					if (unit->GetType() == GUARD && _tilemap->IsEnemyOnTile(g->GetTilePosition()))
					{
						unit->act(_tilemap->GetObjectOnTile(g->GetTilePosition(), ENEMY));
					}
					if (unit->GetType() == ENEMY)
					{
						//unit->SetVisibility(false);
					}

					//If all the objectives are looted and the enemy is at a (de)spawn point, despawn them.
					bool allLootIsCarried = true;
					for (uint k = 0; k < _gameObjects[SPAWN].size(); k++)
					{
						for (uint l = 0; l < _gameObjects[LOOT].size() && allLootIsCarried; l++)
						{
							if (_gameObjects[LOOT][l]->GetPickUpState() == ONTILE || _gameObjects[LOOT][l]->GetPickUpState() == DROPPING)
							{
								allLootIsCarried = false;
							}
						}

						if (unit->GetType() != GUARD &&
							(_gameObjects[LOOT].size() == 0 || allLootIsCarried) &&
							unit->InRange(_gameObjects[SPAWN][k]->GetTilePosition()))
						{
							unit->TakeDamage(10);
							((SpawnPoint*)_gameObjects[SPAWN][k])->AddUnitsToSpawn(1);
							((SpawnPoint*)_gameObjects[SPAWN][k])->Enable();
						}
					}
				}
			}
			else if (g->GetType() == SPAWN)															//Manage enemy spawning
			{
				bool allLootIsCarried = true;
				//Check if all loot is carried
				for (uint l = 0; l < _gameObjects[LOOT].size() && allLootIsCarried; l++)
				{
					if (_gameObjects[LOOT][l]->GetPickUpState() == ONTILE || _gameObjects[LOOT][l]->GetPickUpState() == DROPPING)
					{
						allLootIsCarried = false;
					}
				}

				if (static_cast<SpawnPoint*>(g)->isSpawning() && (_gameObjects[LOOT].size() > 0) && !allLootIsCarried)
				{
					if (Add(_blueprints.GetBlueprintByType(ENEMY,0), 0, g->GetPosition(), g->GetRotation())) //TODO blueprints of spawned enemies should be kept in spawnpoints - Fredrik
					{
						((Unit*)_gameObjects[ENEMY].back())->Move();
					}
				}
			}
			//else if (g->GetType() == TRAP)
			//{
			//	Unit* unit = static_cast<Unit*>(_tilemap->GetUnitOnTile(g->GetTilePosition()._x, g->GetTilePosition()._y));
			//	//Enemy walks over trap
			//	if (unit != nullptr)
			//	{
			//		static_cast<Trap*>(g)->Activate(unit);
			//	}
			//}
		}
	}
	UpdateLights();
}

void ObjectHandler::UpdateLights()
{
	for (pair<GameObject*, Renderer::Spotlight*> spot : _spotlights)
	{
		if (spot.second->IsActive() && spot.first->IsActive())
		{
			if (spot.second->GetBone() != 255)
			{
				spot.second->SetPositionAndRotation(spot.first->GetAnimation()->GetTransforms()[spot.second->GetBone()]);
			}
			else
			{
				XMFLOAT3 pos = spot.first->GetPosition();
				pos.y = 0.5f;

				XMFLOAT3 rot = spot.first->GetRotation();
				rot.x = XMConvertToDegrees(rot.x);
				rot.y = XMConvertToDegrees(rot.y) + 180;
				rot.z = XMConvertToDegrees(rot.z);

				spot.second->SetPositionAndRotation(pos, rot);
			}
		}
	}
}

vector<Blueprint>* ObjectHandler::GetBlueprints()
{
	return _blueprints.GetBlueprints();
}

Blueprint* ObjectHandler::GetBlueprintByName(string name)
{
	return _blueprints.GetBlueprintByName(name);
}

Blueprint* ObjectHandler::GetBlueprintByType(int type, int subType)
{
	return _blueprints.GetBlueprintByType(type, subType);
}

map<GameObject*, Renderer::Spotlight*>* ObjectHandler::GetSpotlights()
{
	return &_spotlights;
}

map<GameObject*, Renderer::Pointlight*>* ObjectHandler::GetPointlights()
{
	return &_pointligths;
}

vector<vector<GameObject*>>* ObjectHandler::GetObjectsInLight(Renderer::Spotlight* spotlight)
{
	return _lightCulling->GetObjectsInSpotlight(spotlight);
}

void ObjectHandler::ReleaseGameObjects()
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
