#include "Settings.h"

namespace Settings
{
	Settings::Settings(int screenWidth, int screenHeight, int shadowMapWidth, int shadowMapHeight, bool fullScreen, bool bordeless, bool showMouseCursor)
	{
		_screenWidth = screenWidth;
		_screenHeight = screenHeight;
		_shadowMapWidth = shadowMapWidth;
		_shadowMapHeight = shadowMapHeight;
		_fullScreen = fullScreen;
		_bordeless = bordeless;
		_showMouseCursor = showMouseCursor;
	}

	Settings::~Settings()
	{
	}
}