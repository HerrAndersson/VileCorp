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
		INT64 _startTime;
		float _ticksPerMs;

		float _tempTime;
		float _frameTime;   //Time since last frame
		float _gameTime;	//Time since the beginning of the game

	public:

		Timer();
		~Timer();

		void Update();

		float GetFrameTime();
		float GetGameTime();

		int GetFPS();

		void Reset();

	};
}

