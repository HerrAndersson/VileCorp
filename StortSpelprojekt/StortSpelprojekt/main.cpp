#include "Game.h"
#include "example.h"

int WINAPI wWinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow)
{
	System::Example* example = new System::Example(5);
	int testVar = example->GetValue();

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