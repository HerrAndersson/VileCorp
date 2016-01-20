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
	vector<string> filenames;
	for (uint a = 0; a < Type::NR_OF_TYPES; a++)
	{
		string folder = "Assets/Gameobjects/";
		folder.append(typeStrings[(Type)a]);
		GetFilenamesInDirectory((char*)folder.c_str(), ".json", filenames);
		data->_objects[a]->resize(filenames.size());
		for (uint i = 0; i < data->_objects[a]->size(); i++)
		{
			if (a = Type::FLOOR)
				System::loadJSON(data->Floors(i), filenames[i]);
			else if (a = Type::WALL)
				System::loadJSON(data->Walls(i), filenames[i]);
			else if (a = Type::TRAP)
				System::loadJSON(data->Traps(i), filenames[i]);
			else if (a = Type::TRIGGER)
				System::loadJSON(data->Triggers(i), filenames[i]);
			else if (a = Type::LOOT)
				System::loadJSON(data->Loot(i), filenames[i]);
			else if (a = Type::GUARD)
				System::loadJSON(data->Guards(i), filenames[i]);
			else if (a = Type::ENEMY)
				System::loadJSON(data->Enemies(i), filenames[i]);
		}
		filenames.clear();
	}
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