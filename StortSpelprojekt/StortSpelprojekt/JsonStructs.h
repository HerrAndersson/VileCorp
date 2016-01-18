#pragma once
#include <cereal/cereal.hpp>

struct PlayerInfo
{
	std::string _name = "Jonas";
	int _gold = 100;
	int _level = 0;
	
	template<class A>
	void serialize(A& a)
	{
		a(CEREAL_NVP(_name)),
		a(CEREAL_NVP(_gold)),
		a(CEREAL_NVP(_level));
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
		a(CEREAL_NVP(_resX)),
		a(CEREAL_NVP(_resY)),
		a(CEREAL_NVP(_default)),
		a(CEREAL_NVP(_shadowmap)),
		a(CEREAL_NVP(_fullScreen)),
		a(CEREAL_NVP(_debugMode)),
		a(CEREAL_NVP(_bordeless)),
		a(CEREAL_NVP(_showMouseCursor));
	}


};

/*

1. Inkludera JsonStructs.h d�r du beh�ver den.
2. Skapa en struct lik PlayerInfo i den h�r .h filel enligt ovan med den data du vill skriva till fil.
3. Skapa ett objekt av din datatyp 
4. Anv�nd sedan i .cpp enligt f�ljande mall:
	
	System::saveJSON(&_dataStruct, "filePath/test.json", "dataStruct Name");
	System::loadJSON(&_dataStruct, "filePath/test.json");

	_dataStruc �r din struct

	"filePath/test.json" �r din filepath

	"dataStruct Name" �r vad containern i filen kommer att heta

*/