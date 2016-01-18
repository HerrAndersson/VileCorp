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

struct ControlMapEdit
{
	struct MapEdit
	{
		std::string rotCamLeft[2] = { "ROTATE_CAMERA_LEFT", "Q" };
		std::string rotCamRight[2] = { "ROTATE_CAMERA_RIGHT", "E" };
		std::string moveCamUp[3] = { "MOVE_CAMERA_UP", "W", "repeat" };
		std::string moveCamDown[3] = { "MOVE_CAMERA_DOWN", "S", "repeat" };
		std::string moveCamLeft[3] = { "MOVE_CAMERA_LEFT", "A", "repeat" };
		std::string moveCamRight[3] = { "MOVE_CAMERA_RIGHT", "D", "repeat" };
		std::string zoomIn[3] = { "ZOOM_CAMERA_IN", "Z", "repeat" };
		std::string zoomOut[3] = { "ZOOM_CAMERA_OUT", "X", "repeat" };
		std::string undo[3] = { "UNDO", "Z", "ctrl" };
		std::string redo[3] = { "REDO", "Y", "ctrl" };
		std::string save[3] = { "SAVE", "S", "ctrl" };
		std::string deleteUnit[2] = { "DELETE_UNIT", "delete"};
	}mapEdit;

	template<class A>
	void serialize(A& a)
	{
		a((CEREAL_NVP(mapEdit.rotCamLeft)),
		(CEREAL_NVP(mapEdit.rotCamRight)),
		(CEREAL_NVP(mapEdit.moveCamUp)),
		(CEREAL_NVP(mapEdit.moveCamDown)),
		(CEREAL_NVP(mapEdit.moveCamLeft)),
		(CEREAL_NVP(mapEdit.moveCamRight)),
		(CEREAL_NVP(mapEdit.zoomIn)),
		(CEREAL_NVP(mapEdit.zoomOut)),
		(CEREAL_NVP(mapEdit.undo)),
		(CEREAL_NVP(mapEdit.redo)),
		(CEREAL_NVP(mapEdit.save)),
		(CEREAL_NVP(mapEdit.deleteUnit)));
	}


};

struct ControlPlay
{
	struct MapEdit
	{
		std::string controlGrp0[3] = { "CONTROL_GROUP0", "0", "ctrl" };
		std::string controlGrp1[3] = { "CONTROL_GROUP1", "1", "ctrl" };
		std::string controlGrp2[3] = { "CONTROL_GROUP2", "2", "ctrl" };
		std::string controlGrp3[3] = { "CONTROL_GROUP3", "3", "ctrl" };
		std::string controlGrp4[3] = { "CONTROL_GROUP4", "4", "ctrl" };
		std::string controlGrp5[3] = { "CONTROL_GROUP5", "5", "ctrl" };
	}mapEdit;

	template<class A>
	void serialize(A& a)
	{
		a((CEREAL_NVP(mapEdit.controlGrp0)),);
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