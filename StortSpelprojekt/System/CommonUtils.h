#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <iostream>
#include <shlobj.h>

namespace System
{
	inline std::string GetMyDocsPath()
	{
		char docsPath[MAX_PATH];
		SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, docsPath);
		return std::string(docsPath);
	}

	inline std::string GetSkirmishFolderPath()
	{
		std::string myDocsPath = GetMyDocsPath();
		myDocsPath += "\\Vile Corp";
		CreateDirectory(myDocsPath.c_str(), NULL);
		myDocsPath += "\\Skirmish Levels";
		CreateDirectory(myDocsPath.c_str(), NULL);
		return myDocsPath + "\\";
	}

	enum Type
	{
		FLOOR, WALL, LOOT, SPAWN, TRAP, CAMERA, GUARD, ENEMY, FURNITURE, NR_OF_TYPES/*Has to be last*/
	};

	const std::string MODEL_FOLDER_PATH = "Assets/Models/";
	const std::wstring TEXTURE_FOLDER_PATH_W = L"Assets/Textures/";
	const std::string LEVEL_FOLDER_PATH = "Assets/Levels/";
	const std::string CAMPAIGN_FOLDER_PATH = LEVEL_FOLDER_PATH + "Campaign/";
	const std::string ANIMATION_FOLDER_PATH = "Assets/Animations/";
	const std::string BLUEPRINTS_PATH = "Assets/blueprints.json";
	const std::string LEVELEDIT_GUI_PATH = "Assets/GUI/leveledit.json";

#ifdef _DEBUG
	const std::string SKIRMISH_FOLDER_PATH = LEVEL_FOLDER_PATH + "Skirmish/";
#else
	const std::string SKIRMISH_FOLDER_PATH = GetSkirmishFolderPath();
#endif

	struct Blueprint
	{
		std::string _name, _tooltip, _mesh;
		Type _type;
		int _subType;
		std::vector<std::string> _textures, _thumbnails;
	};

	struct SpecificBlueprint
	{
		SpecificBlueprint()
		{
			_blueprint = nullptr;
			_textureId = -1;
		}
		void Reset()
		{
			_blueprint = nullptr;
			_textureId = -1;
		}
		Blueprint* _blueprint = nullptr;
		int _textureId = -1;
	};

	struct Hitbox
	{
		float _center[3], _height, _width, _depth;
	};

	inline std::string WStringToString(const std::wstring& ws)
	{
		return std::string(ws.begin(), ws.end());
	}

	inline std::wstring StringToWstring(const std::string& s)
	{
		return std::wstring(s.begin(), s.end());
	}
	/*
		Timer that countdowns 1 each time it's called
		When below stopTime it will reset to startTime
		When above stopTime it will count down
		When equal to stopTime it will return true and then tick down to indicate it's finished
	*/
	static bool FrameCountdown(int& timer, int startTime = 0, int stopTime = 0)
	{
		bool result = false;
		if (timer < stopTime)
		{
			//Start
			timer = startTime;
		}
		else if (timer > stopTime)
		{
			timer--;
		}
		else
		{
			//Finish
			result = true;
			timer--;
		}
		return result;
	}
}