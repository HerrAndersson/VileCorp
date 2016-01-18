#pragma once
#include <cereal/cereal.hpp>
#include <cereal\types\vector.hpp>

struct PlayerInfo
{
	std::string _name = "Jonas";
	int _gold = 100;
	int _level = 0;
	
	template<class A>
	void serialize(A& a)
	{
		a((CEREAL_NVP(_name)),
		(CEREAL_NVP(_gold)),
		(CEREAL_NVP(_level)));
	}

	
};

struct SettingInfo
{
	int _resX = 1920;
	int _resY = 1080;
	bool _default = true;
	bool _shadowmap = true;
	bool _fullScreen = true;
	bool _debugMode = false;
	bool _bordeless = false;
	bool _showMouseCursor = true;

	template<class A>
	void serialize(A& a)
	{
		a((CEREAL_NVP(_default)),
		(CEREAL_NVP(_resX)),
		(CEREAL_NVP(_resY)),
		(CEREAL_NVP(_shadowmap)),
		(CEREAL_NVP(_fullScreen)),
		(CEREAL_NVP(_debugMode)),
		(CEREAL_NVP(_bordeless)),
		(CEREAL_NVP(_showMouseCursor)));
	}


};

/*

1. Inkludera JsonStructs.h där du behöver den.
2. Skapa en struct lik PlayerInfo i den här .h filel enligt ovan med den data du vill skriva till fil.
3. Skapa ett objekt av din datatyp 
4. Använd sedan i .cpp enligt följande mall:
	
	System::saveJSON(&_dataStruct, "filePath/test.json", "dataStruct Name");
	System::loadJSON(&_dataStruct, "filePath/test.json");

	_dataStruc är din struct

	"filePath/test.json" är din filepath

	"dataStruct Name" är vad containern i filen kommer att heta

*/