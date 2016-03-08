#include "ObjectHandler.h"
#include "stdafx.h"

ObjectHandler::ObjectHandler(ID3D11Device* device, AssetManager* assetManager, GameObjectInfo* data, System::Settings* settings, Renderer::ParticleEventQueue* particleEventQueue, System::SoundModule*	soundModule) :
	_currentLevelHeader()
{
	_settings = settings;
	_idCount = 0;
	_assetManager = assetManager;
	_tilemap = new Tilemap();
	_buildingGrid = new Grid(device, 1, 1, 1, XMFLOAT3(1.0f, 1.0f, 0.7f));
	_gameObjectInfo = data;
	_device = device;
	_lightCulling = nullptr;
	_gameObjects.resize(System::NR_OF_TYPES);
	_particleEventQueue = particleEventQueue;
	_soundModule = soundModule;
	_backgroundObject = nullptr;
}

ObjectHandler::~ObjectHandler()
{
	UnloadLevel();
	SAFE_DELETE(_buildingGrid);
	SAFE_DELETE(_backgroundObject);
}

GameObject* ObjectHandler::Add(System::Blueprint* blueprint, int textureId, const XMFLOAT3& position, const XMFLOAT3& rotation, const bool placeOnTilemap, AI::Vec2D direction)
{
	GameObject* object = nullptr;
	System::Type type = (System::Type)blueprint->_type;
	RenderObject* renderObject = _assetManager->GetRenderObject(blueprint->_mesh, blueprint->_textures[textureId]);

	AI::Vec2D tilepos(position.x, position.z);
	XMFLOAT3 lootPos;
	GameObject* loot;

	switch (type)
	{
	case System::WALL:
	case System::FLOOR:
	case System::FURNITURE:
		if (_tilemap->IsTypeOnTile(tilepos, System::LOOT))
		{
			loot = _tilemap->GetObjectOnTile(tilepos, System::LOOT);
			lootPos = loot->GetPosition();
			lootPos.y += 1.2f;
			loot->SetPosition(lootPos);
		}
		object = new Architecture(_idCount, position, rotation, tilepos, type, renderObject, _soundModule, blueprint->_subType, direction);
		break;
	case System::LOOT:
		lootPos = position;
		if (_tilemap->IsTypeOnTile(tilepos, System::FURNITURE))
		{
			lootPos.y += 1.2f;
		}
		object = new Architecture(_idCount, lootPos, rotation, tilepos, type, renderObject, _soundModule, blueprint->_subType, direction);
		break;
	case  System::SPAWN:
		object = new SpawnPoint(_idCount, position, rotation, tilepos, type, renderObject, _soundModule, blueprint->_subType, direction);
		break;
	case  System::TRAP:
		object = new Trap(_idCount, position, rotation, tilepos, type, renderObject, _soundModule, _tilemap, blueprint->_subType, direction);
		break;
	case  System::CAMERA:
		object = new SecurityCamera(_idCount, position, rotation, tilepos, type, renderObject, _soundModule, _tilemap, direction);
		break;
	case  System::ENEMY:
		object = new Enemy(_idCount, position, rotation, tilepos, type, renderObject, _soundModule, _tilemap, blueprint->_subType, direction);
		break;
	case  System::GUARD:
		object = new Guard(_idCount, position, rotation, tilepos, type, renderObject, _soundModule, _tilemap, blueprint->_subType, direction);
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
	if (type == System::TRAP && addedObject && placeOnTilemap)
	{
		Trap* trap = static_cast<Trap*>(object);
		int i = 0;
		AI::Vec2D* arr = trap->GetTiles();
		for (int i = 0; i < trap->GetNrOfOccupiedTiles() && addedObject; i++)
		{
			if (!_tilemap->IsPlaceable(arr[i], blueprint->_type))
			{
				addedObject = false;
			}
		}
		if (addedObject)
		{
			for (int i = 0; i < trap->GetNrOfOccupiedTiles() && addedObject; i++)
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
		if (type == System::GUARD)
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
		if (type == System::CAMERA)
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
		if (type == System::LOOT)
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
		return object;
	}
	return nullptr;
}

bool ObjectHandler::Remove(int ID)
{
	for (int i = 0; i < System::NR_OF_TYPES; i++)
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

bool ObjectHandler::Remove(System::Type type, int ID)
{
	for (uint i = 0; i < _gameObjects[type].size(); i++)
	{
		if (_gameObjects[type][i]->GetID() == ID)
		{
			if (type == System::TRAP)
			{
				AI::Vec2D* tempTiles = static_cast<Trap*>(_gameObjects[type][i])->GetTiles();
				for (int j = 0; j < static_cast<Trap*>(_gameObjects[type][i])->GetNrOfOccupiedTiles(); j++)
				{
					_tilemap->RemoveObjectFromTile(tempTiles[j], _gameObjects[type][i]);
				}
			}
			else
			{
				_tilemap->RemoveObjectFromTile(_gameObjects[type].at(i)->GetTilePosition(), _gameObjects[type].at(i));
			}

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

GameObject* ObjectHandler::Find(System::Type type, int ID)
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

GameObject* ObjectHandler::Find(System::Type type, short index)
{
	for (GameObject* g : _gameObjects[type])
	{
		return g;
	}
	return nullptr;
}

vector<GameObject*>* ObjectHandler::GetAllByType(System::Type type)
{
	return &_gameObjects[type];
}

RenderList ObjectHandler::GetAllByType(int renderObjectID)
{
	// TODO make it so that it returns a vector of renderlists to improve performance /Markus

	RenderList list;
	list._renderObject = _assetManager->GetRenderObject(renderObjectID);

	int count = 0;
	for (int i = 0; i < System::NR_OF_TYPES; i++)
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

int ObjectHandler::GetIdCount()const
{
	return _idCount;
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

	_buildingGrid->ChangeGridSize(_tilemap->GetWidth() - 1, _tilemap->GetHeight() - 1, 1);
}

void ObjectHandler::MinimizeTileMap()
{
	if (_tilemap)
	{
		int minY = -1, maxY = -1;
		for (int y = 0; y < _tilemap->GetHeight() && minY == -1; y++)
		{
			for (int x = 0; x < _tilemap->GetWidth() && minY == -1; x++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					if (y > 0)
					{
						minY = y - 1;
					}
					else
					{
						minY = 0;
					}
				}
			}
		}
		if (minY == -1)
		{
			minY = 0;
		}
		for (int y = _tilemap->GetHeight() - 1; y >= minY && maxY == -1; y--)
		{
			for (int x = 0; x < _tilemap->GetWidth() && maxY == -1; x++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					maxY = y + 2;
				}
			}
		}
		if (maxY == -1)
		{
			maxY = _tilemap->GetHeight();
		}

		int minX = -1, maxX = -1;
		for (int x = 0; x < _tilemap->GetWidth() && minX == -1; x++)
		{
			for (int y = minY; y < maxY && minX == -1; y++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					if (x > 0)
					{
						minX = x - 1;
					}
					else
					{
						minX = 0;
					}
				}
			}
		}
		if (minX == -1)
		{
			minX = 0;
		}
		for (int x = _tilemap->GetWidth() - 1; x >= minX && maxX == -1; x--)
		{
			for (int y = minY; y < maxY && maxX == -1; y++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					maxX = x + 2;
				}
			}
		}
		if (maxX == -1)
		{
			maxX = _tilemap->GetWidth();
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

				std::vector<GameObject*> temp = *_tilemap->GetAllObjectsOnTile(pos);

				for (GameObject* g : temp)
				{
					if (g)
					{
						// Update real pos
						g->SetPosition(XMFLOAT3(x, g->GetPosition().y, y));

						// Update tile
						minimized->AddObjectToTile(x, y, g);
					}
				}
			}
		}

		SAFE_DELETE(_tilemap);
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
				std::vector<GameObject*> temp = *_tilemap->GetAllObjectsOnTile(pos);

				for (GameObject* g : temp)
				{
					if (g)
					{
						// Update real pos
						g->SetPosition(XMFLOAT3(x, g->GetPosition().y, y));

						// Update tile
						large->AddObjectToTile(x, y, g);
					}
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

Level::LevelHeader * ObjectHandler::GetCurrentLevelHeader()
{
	return &_currentLevelHeader;
}

void ObjectHandler::SetCurrentLevelHeader(const Level::LevelHeader& levelheader)
{
	_currentLevelHeader = levelheader;
}

bool ObjectHandler::LoadLevel(const std::string& levelBinaryFilePath)
{
	bool result = false;
	Level::LevelBinary levelData;
	HRESULT success = _assetManager->ParseLevelBinary(&levelData, levelBinaryFilePath);
	result = LoadLevel(levelData, true);
	return result;
}

bool ObjectHandler::LoadLevel(Level::LevelBinary &levelData, bool resizeTileMap)
{
	bool result = true;

	if (resizeTileMap)
	{
		_tilemap = new Tilemap(AI::Vec2D(levelData._tileMapMaxX - levelData._tileMapMinX + 4, levelData._tileMapMaxZ - levelData._tileMapMinZ + 4));
	}

	for (int i = 0; i < levelData._gameObjectData.size() && result; i++)
	{
		std::vector<int>* formattedGameObject = &levelData._gameObjectData[i]; //Structure: { type, subType, textureID, posX, posZ, rot }
		System::Blueprint* blueprint = GetBlueprintByType(formattedGameObject->at(0), formattedGameObject->at(1));

		//Position
		float posX = static_cast<float>(formattedGameObject->at(3));
		float posZ = static_cast<float>(formattedGameObject->at(4));

		if (resizeTileMap)
		{
			posX += 1 - levelData._tileMapMinX;
			posZ += 1 - levelData._tileMapMinZ;
		}

		//Rotation
		float rotY = (formattedGameObject->at(5) * DirectX::XM_PI) / 180.0f;

		AI::Vec2D direction = AI::CLOCKWISE_ROTATION[(formattedGameObject->at(5) * static_cast<int>(8.0f / 360) + 4) % 8];

		GameObject* addedObject = Add(blueprint, formattedGameObject->at(2), DirectX::XMFLOAT3(posX, 0, posZ), DirectX::XMFLOAT3(0, rotY, 0), true, direction);

		//Set no placement zones on floors
		if (addedObject != nullptr && formattedGameObject->at(0) == System::Type::FLOOR && formattedGameObject->size() >= 7)
		{
			static_cast<Architecture*>(addedObject)->SetNoPlacementZone(formattedGameObject->at(6));
		}
	}

	_currentAvailableUnits = levelData._availableUnits;
	_enemySpawnVector = levelData._enemyOrderedSpawnVector;
	_enemySpawnIndex = 0;

	_lightCulling = new LightCulling(_tilemap);

	SAFE_DELETE(_backgroundObject);
	int sizeX = 90 + _tilemap->GetWidth();
	int sizeY = 90 + _tilemap->GetHeight();

	CreateBackgroundObject(sizeX, sizeY, "grass1.png", sizeX / 3, sizeY / 3);
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

void ObjectHandler::Update(float deltaTime)
{
	//Update all objects' gamelogic
	for (int i = 0; i < System::NR_OF_TYPES; i++)
	{
		for (unsigned int j = 0; j < _gameObjects[i].size(); j++)
		{
			GameObject* g = _gameObjects[i][j];
			g->Update(deltaTime);

			if (g->GetPickUpState() == PICKEDUP)
			{
				_tilemap->RemoveObjectFromTile(g->GetTilePosition(), g);
				g->SetPickUpState(HELD);
			}
			else if (g->GetPickUpState() == DROPPING)
			{
				_tilemap->AddObjectToTile(g->GetTilePosition(), g);
				g->SetPickUpState(ONTILE);
				((Architecture*)g)->SetTargeted(false);
			}

			if (g->GetType() == System::GUARD || g->GetType() == System::ENEMY)
			{
				Unit* unit = static_cast<Unit*>(g);
				GameObject* heldObject = unit->GetHeldObject();
				if (heldObject != nullptr && heldObject->GetPickUpState() == HELD)
				{
					heldObject->SetPosition(DirectX::XMFLOAT3(unit->GetPosition().x, unit->GetPosition().y + 2.0f, unit->GetPosition().z));
					heldObject->SetTilePosition(AI::Vec2D(heldObject->GetPosition().x, heldObject->GetPosition().z));
				}

				bool inRangeOfSpawn = false;
				for (uint k = 0; k < _gameObjects[System::SPAWN].size() && !inRangeOfSpawn; k++)
				{
					if (!_gameObjects[System::SPAWN][k]->InRange(unit->GetTilePosition()))
					{
						static_cast<Enemy*>(unit)->SetIsAtSpawn(false);
					}
					else
					{
						static_cast<Enemy*>(unit)->SetIsAtSpawn(true);
						inRangeOfSpawn = true;
					}
				}

				//Show Unit Lifebar
				if (unit->GetHealth() > 0 && unit->IsVisible())
				{
					XMFLOAT3 pos = unit->GetPosition();
					pos.y += 2.5f;

					ParticleRequestMessage* msg = new ParticleRequestMessage(ParticleType::ICON, ParticleSubType::HEALTH_SUBTYPE, -1, pos, XMFLOAT3(0, 0, 0), 1.0f, 1, unit->GetHealth()*0.0025f, true, true);
					GetParticleEventQueue()->Insert(msg);
				}

				if (unit->GetHealth() <= 0 && unit->GetAnimisFinished())
				{
					if (unit->IsSwitchingTile())
					{
						unit->SetTilePosition(unit->GetNextTile());
					}

					if (heldObject != nullptr)
					{
						bool lootRemoved = false;
						for (uint k = 0; k < _gameObjects[System::SPAWN].size() && !lootRemoved; k++)
						{
							//If the enemy is at the despawn point with an objective, remove the objective and the enemy, Aron
							if (_gameObjects[System::SPAWN][k]->InRange(unit->GetTilePosition()))
							{
								lootRemoved = Remove(heldObject);

								if (_tilemap->GetNrOfLoot() > 0)
								{
									_enemySpawnVector.push_back(std::array<int, 2>{_enemySpawnVector.back()[0] + 1, (int)unit->GetSubType()});
								}
							}
						}

						if (!lootRemoved)
						{
							heldObject->SetPickUpState(DROPPING);
							heldObject->SetPosition(XMFLOAT3(heldObject->GetPosition().x, 0.0f, heldObject->GetPosition().z));
						}
					}

					//Avoids the case where the Guard tries to attack a scrap value when the Enemy has been taken care of, Aron
					for (uint k = 0; k < _gameObjects[System::GUARD].size(); k++)
					{
						if (((Guard*)_gameObjects[System::GUARD][k])->GetObjective() != nullptr)
						{
							if (((Guard*)_gameObjects[System::GUARD][k])->GetObjective()->GetID() == g->GetID())
							{
								((Guard*)_gameObjects[System::GUARD][k])->ClearObjective();
							}
						}
					}

					//Avoids the case where the Enemy tries to attack a scrap value when the Guard has been taken care of, Aron
					for (uint k = 0; k < _gameObjects[System::ENEMY].size(); k++)
					{
						if (((Enemy*)_gameObjects[System::ENEMY][k])->GetObjective() != nullptr)
						{
							if (((Enemy*)_gameObjects[System::ENEMY][k])->GetObjective()->GetID() == g->GetID())
							{
								((Enemy*)_gameObjects[System::ENEMY][k])->ClearObjective();
							}
						}
					}

					if (!static_cast<Enemy*>(unit)->GetIsAtSpawn())
					{
						//Bloodparticles on death
						ParticleRequestMessage* msg = new ParticleRequestMessage(ParticleType::SPLASH, ParticleSubType::BLOOD_SUBTYPE, -1, unit->GetPosition(), XMFLOAT3(0, 1, 0), 300.0f, 200, 0.1f, true);
						GetParticleEventQueue()->Insert(msg);

						//Play death sound
						float x = g->GetPosition().x;
						float z = g->GetPosition().z;
						if (g->GetType() == System::ENEMY)
						{
							_soundModule->SetSoundPosition("enemy_death", x, 0.0f, z);
							_soundModule->Play("enemy_death");
						}
						else if (g->GetType() == System::GUARD)
						{
							_soundModule->SetSoundPosition("guard_death", x, 0.0f, z);
							_soundModule->Play("guard_death");
						}
					}

					//Remove object
					Remove(g);
					g = nullptr;
					unit = nullptr;
					j--;
				}
				else if (unit->IsSwitchingTile())
				{
					_tilemap->RemoveObjectFromTile(unit->GetTilePosition(), g);
					_tilemap->AddObjectToTile(unit->GetNextTile(), g);

					//If all the objectives are looted and the enemy is at a (de)spawn point, despawn them.
					bool allLootIsCarried = true;
					for (uint k = 0; k < _gameObjects[System::SPAWN].size() && !allLootIsCarried; k++)
					{
						for (uint l = 0; l < _gameObjects[System::LOOT].size() && allLootIsCarried; l++)
						{
							if (_gameObjects[System::LOOT][l]->GetPickUpState() == ONTILE || _gameObjects[System::LOOT][l]->GetPickUpState() == DROPPING)
							{
								allLootIsCarried = false;
							}
						}

						if (unit->GetType() == System::ENEMY &&
							(_gameObjects[System::LOOT].size() == 0 || allLootIsCarried) &&
							_gameObjects[System::SPAWN][k]->InRange(unit->GetTilePosition()))
						{
							unit->TakeDamage(unit->GetHealth());
						}
					}

					if (allLootIsCarried && unit->GetHeldObject() == nullptr && static_cast<Enemy*>(unit)->GetMoveState() != Unit::MoveState::FLEEING)
					{
						static_cast<Enemy*>(unit)->CheckAllTiles();
					}
				}
			}
		}
	}
	if (_spawnTimer % 60 == 0)
	{
		SpawnEnemies();
	}
	_spawnTimer++;
	UpdateLights();
}

void ObjectHandler::SpawnEnemies()
{
	if (_enemySpawnVector.size() > 0 && _enemySpawnIndex < _enemySpawnVector.size())
	{
		int nextEnemySpawnTime = _enemySpawnVector[_enemySpawnIndex][0];

		if (_spawnTimer >= nextEnemySpawnTime * 60)
		{
			int nextEnemyType = _enemySpawnVector[_enemySpawnIndex][1];
			System::Blueprint* nextEnemyBlueprint = _blueprints.GetBlueprintByType(System::ENEMY, nextEnemyType);

			bool enemyAdded = false;
			int initialSpawnPointIndex = 0;
			if (_gameObjects[System::Type::SPAWN].size() > 1)
			{
				initialSpawnPointIndex = rand() % (_gameObjects[System::Type::SPAWN].size() - 1);
			}
			int currentSpawnPointIndex = initialSpawnPointIndex;

			do
			{
				GameObject* selectedSpawnPoint = _gameObjects[System::Type::SPAWN][currentSpawnPointIndex];
				if (!_tilemap->IsEnemyOnTile(selectedSpawnPoint->GetTilePosition()))
				{
					enemyAdded = Add(nextEnemyBlueprint, 0, selectedSpawnPoint->GetPosition(), selectedSpawnPoint->GetRotation(), true);
				}
				else
				{
					currentSpawnPointIndex++;
					if (_gameObjects[System::Type::SPAWN].size() <= currentSpawnPointIndex)
					{
						currentSpawnPointIndex = 0;
					}
				}
			} while (!enemyAdded && currentSpawnPointIndex != initialSpawnPointIndex);

			if (enemyAdded)
			{
				_enemySpawnIndex++;
				SpawnEnemies();
			}
		}
	}
}

void ObjectHandler::UpdateLights()
{
	for (pair<GameObject*, Renderer::Spotlight*> spot : _spotlights)
	{
		if (spot.first == nullptr)
		{
			SAFE_DELETE(spot.second);
			spot.second = nullptr;
		}
		else
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
	for (pair<GameObject*, Renderer::Pointlight*> point : _pointligths)
	{
		if (point.first == nullptr)
		{
			SAFE_DELETE(point.second);
			point.second = nullptr;
		}
		else
		{
			if (point.first->IsActive() && point.first->IsVisible())
			{
				XMFLOAT3 pos = point.first->GetPosition();
				point.second->SetPosition(DirectX::XMFLOAT3(pos.x, 2, pos.z));
				point.second->SetActive(true);
			}
			else
			{
				point.second->SetActive(false);
			}
		}
	}
}

vector<System::Blueprint>* ObjectHandler::GetBlueprints()
{
	return _blueprints.GetBlueprints();
}

std::vector<std::vector<System::Blueprint*>>* ObjectHandler::GetBlueprintsOrderedByType()
{
	return _blueprints.GetBlueprintsOrderedByType();
}

System::Blueprint* ObjectHandler::GetBlueprintByName(const std::string& name)
{
	return _blueprints.GetBlueprintByName(name);
}

System::Blueprint* ObjectHandler::GetBlueprintByType(int type, int subType)
{
	return _blueprints.GetBlueprintByType(type, subType);
}

std::vector<std::string>* ObjectHandler::GetCurrentAvailableUnits()
{
	return &_currentAvailableUnits;
}

int ObjectHandler::GetRemainingToSpawn()
{
	return _enemySpawnVector.size() - _enemySpawnIndex;
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

RenderObject* ObjectHandler::GetBackgroundObject()
{
	return _backgroundObject;
}

//If the texture should repeat for every tile, set texRepeatCountX = sizeX and texRepeatCountY = sizeY
void ObjectHandler::CreateBackgroundObject(const float& sizeX, const float& sizeY, const std::string& textureName, const int& texRepeatCountX, const int& texRepeatCountY)
{
	_backgroundObject = new RenderObject();

	float left = -1 * sizeX;
	float right = 1 * sizeY;
	float top = 1 * sizeY;
	float bottom = -1 * sizeY;
	XMFLOAT3 normal = XMFLOAT3(0, 1, 0);

	Vertex quad[] =
	{ { XMFLOAT3(left, -0.01f, top), normal, XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(right, -0.01f, bottom), normal, XMFLOAT2(texRepeatCountX, texRepeatCountY) },
	{ XMFLOAT3(left, -0.01f, bottom), normal, XMFLOAT2(0.0f, texRepeatCountY) },
	{ XMFLOAT3(left, -0.01f, top), normal, XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(right, -0.01f, top), normal, XMFLOAT2(texRepeatCountX, 0.0f) },
	{ XMFLOAT3(right, -0.01f, bottom), normal, XMFLOAT2(texRepeatCountX, texRepeatCountY) } };

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * 6;

	Mesh* mesh = new Mesh();

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = quad;
	HRESULT result = _device->CreateBuffer(&bufferDesc, &data, &mesh->_vertexBuffer);

	mesh->_activeUsers = 1;
	mesh->_hitbox = nullptr;
	mesh->_isSkinned = false;
	mesh->_meshLoaded = true;
	mesh->_name = "background";
	mesh->_pointLights.clear();
	mesh->_skeleton = nullptr;
	mesh->_skeletonName = "nullptr";
	mesh->_spotLights.clear();
	mesh->_toMesh = 1;
	mesh->_vertexBufferSize = 6;

	_backgroundObject->_diffuseTexture = _assetManager->GetTexture(textureName);
	_backgroundObject->_specularTexture = nullptr;
	_backgroundObject->_mesh = mesh;
}

void ObjectHandler::ReleaseGameObjects()
{
	for (int i = 0; i < System::NR_OF_TYPES; i++)
	{
		for (GameObject* g : _gameObjects[i])
		{
			g->Release();
			SAFE_DELETE(g);
		}
		_gameObjects[i].clear();
	}
	_idCount = 0;
	_objectCount = 0;
}

Renderer::ParticleEventQueue* ObjectHandler::GetParticleEventQueue()
{
	return _particleEventQueue;
}
