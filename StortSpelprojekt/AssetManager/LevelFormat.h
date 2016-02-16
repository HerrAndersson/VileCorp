#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <cereal\archives\binary.hpp>
#include <cereal\archives\json.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>

struct LevelHeader
{
	std::string _description = "";
	int _budget = 0;
	std::string _levelBinaryFilename = "";

	template<class A>
	void serialize(A& a)
	{
		a(
			CEREAL_NVP(_description),
			CEREAL_NVP(_budget),
			CEREAL_NVP(_levelBinaryFilename)
			);
	}
};

struct LevelBinary
{
	int _tileMapSizeX = 0;
	int _tileMapSizeZ = 0;
	std::vector<std::vector<int>> _gameObjectData = std::vector<std::vector<int>>();

	template<class A>
	void serialize(A& a)
	{
		a(
			CEREAL_NVP(_tileMapSizeX),
			CEREAL_NVP(_tileMapSizeZ),
			CEREAL_NVP(_gameObjectData)
			);
	}
};