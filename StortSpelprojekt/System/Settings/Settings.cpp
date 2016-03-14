#include "settings.h"
namespace System
{
	Settings::Settings()
	{
		_screenWidth = 1600;
		_screenHeight = 900;
		_windowWidth = 1920;
		_windowHeight = 1080;
		_shadowMapSize = 256;
		_screenMode = 2;
		_showMouseCursor = true;
		_nearClip = 0.1f;
		_farClip = 1000.0f;
		_fov = 1.57079f; //almost PI/2;
		_antialiasing = true;
		_volume = 100;
		_brightness = 5;
	}

	Settings::~Settings()
	{
	}
}
