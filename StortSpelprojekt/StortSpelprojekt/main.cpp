#include "Game.h"

int WINAPI wWinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow)
{
	int result = 0;

	//_CrtSetBreakAlloc(232);

	try
	{
		myInitMemoryCheck();
		
		Game game(_hInstance, _nCmdShow);
		result = game.Run();
	}
	catch (const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Error", MB_ICONERROR | MB_OK);
		result = 1;
	}
	return result;
}