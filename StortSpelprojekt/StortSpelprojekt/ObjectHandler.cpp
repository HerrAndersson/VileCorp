#include "ObjectHandler.h"
#include "stdafx.h"

ObjectHandler::ObjectHandler(ID3D11Device* device, AssetManager* assetManager, GameObjectInfo* data, System::Settings* settings)
{
	_settings = settings;
	_idCount = 0;
	_assetManager = assetManager;
	_tilemap = nullptr;
	_buildingGrid = new Grid(device, 1, 1, 1, XMFLOAT3(1.0f, 1.0f, 0.7f));
	_gameObjectInfo = data;
	_device = device;
	_lightCulling = nullptr;

	ActivateTileset("default2");
}

ObjectHandler::~ObjectHandler()
{
	UnloadLevel();
	SAFE_DELETE(_buildingGrid);
}

bool ObjectHandler::LoadLevel(int lvlIndex)
{
	int dimX, dimY;
	vector<GameObjectData> gameObjectData;
	bool result = _assetManager->ParseLevel(lvlIndex, gameObjectData, dimX, dimY);
	if (result)
	{
		if (_gameObjects.size() < 1)
		{
			for (int i = 0; i < NR_OF_TYPES; i++)
			{
				_gameObjects.push_back(std::vector<GameObject*>());
			}
		}

		delete _tilemap;
		_tilemap = new Tilemap(AI::Vec2D(dimX, dimY));

		for (auto i : gameObjectData)
		{
			Add((Type)i._tileType, 0, DirectX::XMFLOAT3(i._posX, 0, i._posZ), DirectX::XMFLOAT3(0, i._rotY, 0));
		}
	}

	_lightCulling = new LightCulling(_tilemap);

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

	ReleaseGameObjects();
	SAFE_DELETE(_tilemap);
	SAFE_DELETE(_lightCulling);
}

void ObjectHandler::ActivateTileset(const string& name)
{
	ReleaseGameObjects();
	_assetManager->ActivateTileset(name);

	for (uint a = 0; a < Type::NR_OF_TYPES; a++)
	{
		for (uint i = 0; i < _gameObjectInfo->_objects[a]->size(); i++)
		{
			_gameObjectInfo->_objects[a]->at(i)->_renderObject = _assetManager->GetRenderObjectByType((Type)a, i);
		}
	}
}

bool ObjectHandler::Add(Type type, int index, const XMFLOAT3& position, const XMFLOAT3& rotation, const int subIndex, const bool blueprint)
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
	case TRAP:
		object = MakeTrap(_gameObjectInfo->Traps(index), position, rotation, subIndex);
		break;
	case CAMERA:
		object = MakeSecurityCamera(_gameObjectInfo->Cameras(index), position, rotation);
		break;
	case ENEMY:
		object = MakeEnemy(_gameObjectInfo->Enemies(index), position, rotation);
		break;
	case GUARD:
		object = MakeGuard(_gameObjectInfo->Guards(index), position, rotation);
		break;
	case FURNITURE:
		object = MakeFurniture(_gameObjectInfo->Furnitures(index), position, rotation);
		break;
	default:	
		break;
	}

	if (!blueprint)
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
	if (type == TRAP && addedObject && !blueprint)
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

		//for(auto i : object->GetRenderObject()->_mesh._spotLights)
		//{
		//	_spotlights[object] = new Renderer::Spotlight(_device, i, 0.1f, 1000.0f);
		//}
		_objectCount++;
		return true;
	}

	return false;
}

bool ObjectHandler::Add(Type type, const std::string& name, const XMFLOAT3& position = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rotation = XMFLOAT3(0.0f, 0.0f, 0.0f), const int subIndex, const bool blueprint)
{
	for (unsigned int i = 0; i < _gameObjectInfo->_objects[type]->size(); i++)
	{
		if (_gameObjectInfo->_objects[type]->at(i)->_name == name)
		{
			return Add(type, i, position, rotation, subIndex, blueprint);
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

				if (unit->GetHealth() <= 0)
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
						}
					}
				}
			}
			else if (g->GetType() == SPAWN)															//Manage enemy spawning
			{
				if (static_cast<SpawnPoint*>(g)->isSpawning())
				{
					if (Add(ENEMY, "enemy_proto", g->GetPosition(), g->GetRotation()))
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
	if (_gameObjects.size() > 0)
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
		_gameObjects.clear();
	}
	_idCount = 0;
	_objectCount = 0;
}


/*Make object*/

Architecture * ObjectHandler::MakeFloor(GameObjectFloorInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Architecture* obj = new Architecture(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		FLOOR,
		_assetManager->GetRenderObject(data->_renderObject));

	return obj;
}

Architecture * ObjectHandler::MakeWall(GameObjectWallInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Architecture* obj = new Architecture(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		WALL,
		_assetManager->GetRenderObject(data->_renderObject));

	return obj;
}

Architecture * ObjectHandler::MakeFurniture(GameObjectFurnitureInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Architecture* obj = new Architecture(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		FURNITURE,
		_assetManager->GetRenderObject(13));

	return obj;
}

Architecture * ObjectHandler::MakeLoot(GameObjectLootInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Architecture* obj = new Architecture(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		LOOT,
		_assetManager->GetRenderObject(data->_renderObject));

	// read more data

	return obj;
}

SpawnPoint * ObjectHandler::MakeSpawn(GameObjectSpawnInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	SpawnPoint* obj = new SpawnPoint(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		SPAWN,
		_assetManager->GetRenderObject(4),
		180, 2);

	// read more data

	return obj;
}

Trap * ObjectHandler::MakeTrap(GameObjectTrapInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation, const int subIndex)
{
	Trap* obj = new Trap(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		TRAP,
		_assetManager->GetRenderObject(data->_renderObject),
		_tilemap,
		subIndex,
		{ 1,0 },
		data->_cost);

	// Read more data

	return obj;
}

SecurityCamera*	ObjectHandler::MakeSecurityCamera(GameObjectCameraInfo* data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	SecurityCamera* obj = new SecurityCamera(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		CAMERA,
		_assetManager->GetRenderObject(data->_renderObject),
		_tilemap);

	// Read more data

	return obj;
}

Guard * ObjectHandler::MakeGuard(GameObjectGuardInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Guard* obj = new Guard(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		GUARD,
		_assetManager->GetRenderObject(data->_renderObject),
		_tilemap);

	// read more data

	return obj;
}

Enemy * ObjectHandler::MakeEnemy(GameObjectEnemyInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Enemy* obj = new Enemy(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		ENEMY,
		_assetManager->GetRenderObject(data->_renderObject),
		_tilemap);

	// read more data

	return obj;
}
