#include "Game.h"

int WINAPI wWinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow)
{
	int result = 0;

	//_CrtSetBreakAlloc(55100);

	try
	{
		myInitMemoryCheck();
		
		Game game(_hInstance, _nCmdShow);
		result = game.Run();
	}
	catch (const std::exception& e)
	{
		MessageBox(NULL, e.what(), "Error", MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
		result = 1;
	}
	return result;
}