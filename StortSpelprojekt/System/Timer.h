#pragma once
#include <windows.h>
#include <stdexcept>

#define SYSTEM_EXPORT __declspec(dllexport)
namespace System
{
	class SYSTEM_EXPORT Timer
	{
	private:

		INT64 _frequency;
		double _ticksPerMs;
		INT64 _startTime;

		double _tempTime;
		double _frameTime;   //Time since last frame
		double _gameTime;	//Time since the beginning of the game

	public:

		Timer();
		~Timer();

		void Update();

		double GetFrameTime();
		double GetGameTime();

		int GetFPS();

		void Reset();

	};
}

