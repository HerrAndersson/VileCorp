#pragma once
#include <Windows.h>
#define SYSTEM_EXPORT __declspec(dllexport)

namespace System
{
	class SYSTEM_EXPORT Window
	{

	private:

		LPCSTR		_applicationName;
		HINSTANCE	_hinstance;
		HWND		_hwnd;

		bool _fullscreen;
		bool _showCursor;

		int _windowWidth;
		int _windowHeight;

		void InitializeWindow();
		void ShutdownWindow();

	public:

		Window();
		Window(LPCSTR applicationName, HINSTANCE hinstance, bool fullscreen = false, bool showCursor = true, int windowWidth = 1000, int windowHeight = 800);
		virtual ~Window();

		void Run();

		LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	};

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static Window* windowHandle = nullptr;
}
