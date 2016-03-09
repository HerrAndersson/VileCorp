#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cereal\archives\binary.hpp>
#include <cereal\archives\json.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\array.hpp>

namespace Level
{
	enum GameModes { KILL_THEM_ALL, SURVIVAL };
	static const char * GameModeNames[] = { "Kill them all", "Survival" };

	struct LevelHeader
	{
		std::string _storyTitle = "";
		std::string _storyBody = "";
		int _budget = 0;
		int _gameMode = 0;
		int _surviveForSeconds = 0; //Specifically for the survival gamemode 
		std::string _levelBinaryFilename = "";
		bool _isCampaignMode = false;
		int _campaignLevelIndex = 0;

		template<class A>
		void serialize(A& a)
		{
			a(
				CEREAL_NVP(_storyTitle),
				CEREAL_NVP(_storyBody),
				CEREAL_NVP(_budget),
				CEREAL_NVP(_gameMode),
				CEREAL_NVP(_surviveForSeconds),
				CEREAL_NVP(_levelBinaryFilename)
				);
		}
	};

	struct LevelBinary
	{
		int _tileMapMaxX = 0;
		int _tileMapMaxZ = 0;
		int _tileMapMinX = 0;
		int _tileMapMinZ = 0;
		std::vector<std::vector<int>> _gameObjectData = std::vector<std::vector<int>>();
		std::vector<std::vector<int>> _enemyWavesGUIData = std::vector<std::vector<int>>();
		std::vector<std::array<int, 2>> _enemyOrderedSpawnVector = std::vector<std::array<int, 2>>();
		std::vector<std::string> _availableUnits = std::vector<std::string>();

		template<class A>
		void serialize(A& a)
		{
			a(
				CEREAL_NVP(_tileMapMaxX),
				CEREAL_NVP(_tileMapMaxZ),
				CEREAL_NVP(_tileMapMinX),
				CEREAL_NVP(_tileMapMinZ),
				CEREAL_NVP(_gameObjectData),
				CEREAL_NVP(_enemyWavesGUIData),
				CEREAL_NVP(_enemyOrderedSpawnVector),
				CEREAL_NVP(_availableUnits)
				);
		}
	};
}