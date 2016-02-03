#include "Timer.h"

using namespace std;

namespace System
{
	Timer::Timer()
	{
		// Check to see if this system supports high performance timers
		QueryPerformanceFrequency((LARGE_INTEGER*)&_frequency);

		if (_frequency == 0)
		{
			throw runtime_error("Timer: Could not initialize Timer");
		}

		_ticksPerMs = (float)(_frequency / 1000);

		QueryPerformanceCounter((LARGE_INTEGER*)&_startTime);

		_frameTime = 0;
		_gameTime = 0;
		_frequency = 0;
		_startTime = 0;
		_tempTime = 0;
	}

	Timer::~Timer()
	{
	}

	void Timer::Update()
	{
		INT64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);		//Query the current time
		double timeDifference = (double)(currentTime - _startTime); 	//Difference in time since the last time-query 
		_tempTime = timeDifference / _ticksPerMs;						//Time difference over the timer speed resolution give frameTime
		_startTime = currentTime; 									//Restart the timer

		_frameTime += _tempTime;
		_gameTime += _tempTime;

		if (_gameTime > (DBL_MAX - 100000000))
		{
			_gameTime = 0;
		}
	}

	// Seconds
	double Timer::GetFrameTime()
	{
		return _frameTime;
	}

	double Timer::GetGameTime()
	{
		return _gameTime;
	}

	void Timer::Reset()
	{
		_frameTime = 0;
	}

	int Timer::GetFPS()
	{
		return (int)(1 / (_frameTime / 1000));
	}
}