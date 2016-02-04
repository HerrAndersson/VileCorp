#include "settings.h"
namespace System
{
	Settings::Settings()
	{
		_screenWidth = 1920;
		_screenHeight = 1080;
		_shadowMapWidth = 256;
		_shadowMapHeight = 256;
		_fullscreen = false;
		_borderless = false;
		_showMouseCursor = true;
		_nearClip = 0.1f;
		_farClip = 1000.0f;
		_fov = 1.57079f; //almost PI/2;
	}
	Settings::Settings(int screenWidth, int screenHeight, int shadowMapWidth, int shadowMapHeight, bool fullscreen, bool borderless, bool showMouseCursor, float nearClip, float farClip, float fov)
	{
		_screenWidth = screenWidth;
		_screenHeight = screenHeight;
		_shadowMapWidth = shadowMapWidth;
		_shadowMapHeight = shadowMapHeight;
		_fullscreen = fullscreen;
		_borderless = borderless;
		_showMouseCursor = showMouseCursor;
		_nearClip = nearClip;
		_farClip = farClip;
		_fov = fov;
	}

	Settings::~Settings()
	{
	}
}
