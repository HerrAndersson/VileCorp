#pragma once
#include <string>

namespace System
{
	enum Type
	{
		FLOOR, WALL, LOOT, SPAWN, TRAP, CAMERA, GUARD, ENEMY, FURNITURE, NR_OF_TYPES/*Has to be last*/
	};

	const std::string MODEL_FOLDER_PATH = "Assets/Models/";
	const std::wstring TEXTURE_FOLDER_PATH_W = L"Assets/Textures/";
	const std::string LEVEL_FOLDER_PATH = "Assets/Levels/Format12/";
	const std::string ANIMATION_FOLDER_PATH = "Assets/Animations/";
	const std::string BLUEPRINTS_PATH = "Assets/blueprints.json";

	struct Hitbox
	{
		float _center[3], _height, _width, _depth;
	};

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