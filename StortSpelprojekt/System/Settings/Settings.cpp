#include "settings.h"
namespace System
{
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
