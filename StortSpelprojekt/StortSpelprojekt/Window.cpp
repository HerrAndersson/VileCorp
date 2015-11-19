#include "Window.h"
namespace System
{
	Window::Window() {}

	Window::Window(LPCSTR applicationName, HINSTANCE hinstance, bool fullscreen, bool showCursor, int windowWidth, int windowHeight)
	{
		_applicationName = applicationName;
		_hinstance = hinstance;
		_fullscreen = fullscreen;
		_showCursor = showCursor;
		_windowWidth = windowWidth;
		_windowHeight = windowHeight;

		InitializeWindow();
	}

	Window::~Window()
	{
		ShutdownWindow();
	}

	void Window::ShutdownWindow()
	{
		ShowCursor(true);

		if (_fullscreen)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);

		DestroyWindow(_hwnd);
		_hwnd = NULL;

		UnregisterClass(_applicationName, _hinstance);
		_hinstance = NULL;
	}

	void Window::InitializeWindow()
	{
		//windowHandle = this;

		int posX, posY;

		//_hinstance = GetModuleHandle(NULL);

		//Setup the windows class with default settings.
		WNDCLASSEX wc;
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = _hinstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = _applicationName;
		wc.cbSize = sizeof(WNDCLASSEX);

		RegisterClassEx(&wc);

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		//Setup the screen settings depending on whether it is running in full screen or in windowed mode.
		if (_fullscreen)
		{
			_windowHeight = screenHeight;
			_windowWidth = screenWidth;

			//If full screen set the screen to maximum size of the users desktop and 32bit.
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
			dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			posX = 0;
			posY = 0;
		}
		else //If windowed
		{
			//Place the window in the middle of the screen.
			posX = (screenWidth - _windowWidth) / 2;
			posY = (screenHeight - _windowHeight) / 2;
		}

		RECT rc = { 0, 0, _windowWidth, _windowHeight };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		_hwnd = CreateWindowEx(WS_EX_APPWINDOW, _applicationName, _applicationName,
			WS_OVERLAPPEDWINDOW | CW_USEDEFAULT | CW_USEDEFAULT,
			posX, posY, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, _hinstance, NULL);

		ShowWindow(_hwnd, SW_SHOW);
		SetForegroundWindow(_hwnd);
		SetFocus(_hwnd);

		SetCursorPos(screenWidth / 2, screenHeight / 2);
		ShowCursor(_showCursor);
	}

	void Window::Run()
	{
		MSG msg;
		bool result;
		bool done = false;

		ZeroMemory(&msg, sizeof(MSG));

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	LRESULT CALLBACK Window::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
	{
		switch (umessage)
		{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return windowHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
		}
	}
}