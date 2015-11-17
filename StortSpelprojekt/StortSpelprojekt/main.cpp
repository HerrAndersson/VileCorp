#include "Game.h"

int WINAPI wWinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow)
{
	myInitMemoryCheck();
	int rv = 0;

	Game game;

	rv = game.Initialize(_hInstance, _nCmdShow);
	if (FAILED(rv))
	{
		return rv;
	}

	rv = game.Run();

	return rv;
}