#pragma once
#include "JsonStructs.h"
#include "JsonParser.h"

class GameObjectDataLoader
{
public:
	GameObjectDataLoader();
	~GameObjectDataLoader();

	bool LoadGameObjectInfo(GameObjectInfo* data);
	void WriteSampleGameObjects();
};

inline GameObjectDataLoader::GameObjectDataLoader()
{
};

inline GameObjectDataLoader::~GameObjectDataLoader()
{
};

inline bool GameObjectDataLoader::LoadGameObjectInfo(GameObjectInfo* data)
{
	vector<std::string> filenames;
	GetFilenamesInDirectory("Assets/Gameobjects/Floors/", ".json", filenames);
	data->_floors.resize(filenames.size());
	for (uint b = 0; b < filenames.size(); b++)
	{
		System::loadJSON(&data->_floors[b], filenames[b]);
	}
	filenames.clear();
	GetFilenamesInDirectory("Assets/Gameobjects/Walls/", ".json", filenames);
	data->_walls.resize(filenames.size());
	for (uint b = 0; b < filenames.size(); b++)
	{
		System::loadJSON(&data->_walls[b], filenames[b]);
	}
	filenames.clear();
	GetFilenamesInDirectory("Assets/Gameobjects/Loot/", ".json", filenames);
	data->_loot.resize(filenames.size());
	for (uint b = 0; b < filenames.size(); b++)
	{
		System::loadJSON(&data->_loot[b], filenames[b]);
	}
	filenames.clear();
	GetFilenamesInDirectory("Assets/Gameobjects/Traps/", ".json", filenames);
	data->_traps.resize(filenames.size());
	for (uint b = 0; b < filenames.size(); b++)
	{
		System::loadJSON(&data->_traps[b], filenames[b]);
	}
	filenames.clear();
	GetFilenamesInDirectory("Assets/Gameobjects/Triggers/", ".json", filenames);
	data->_triggers.resize(filenames.size());
	for (uint b = 0; b < filenames.size(); b++)
	{
		System::loadJSON(&data->_triggers[b], filenames[b]);
	}
	filenames.clear();
	GetFilenamesInDirectory("Assets/Gameobjects/Guards/", ".json", filenames);
	data->_guards.resize(filenames.size());
	for (uint b = 0; b < filenames.size(); b++)
	{
		System::loadJSON(&data->_guards[b], filenames[b]);
	}
	filenames.clear();
	GetFilenamesInDirectory("Assets/Gameobjects/Enemies/", ".json", filenames);
	data->_enemies.resize(filenames.size());
	for (uint b = 0; b < filenames.size(); b++)
	{
		System::loadJSON(&data->_enemies[b], filenames[b]);
	}
	filenames.clear();

	return false;
}

inline void GameObjectDataLoader::WriteSampleGameObjects()
{
	{
		GameObjectFloorInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Floors/sample.json", "sample");
	}
	{
		GameObjectWallInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Walls/sample.json", "sample");
	}
	{
		GameObjectLootInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Loot/sample.json", "sample");
	}
	{
		GameObjectTrapInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Traps/sample.json", "sample");
	}
	{
		GameObjectTriggerInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Triggers/sample.json", "sample");
	}
	{
		GameObjectGuardInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Guards/sample.json", "sample");
	}
	{
		GameObjectEnemyInfo sample;
		System::saveJSON(&sample, "Assets/Gameobjects/Enemies/sample.json", "sample");
	}
};