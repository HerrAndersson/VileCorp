#include "Window.h"
namespace System
{
	Window::Window(LPCSTR applicationName, HINSTANCE hinstance, System::Settings* settings, WNDPROC wndProc)
	{
		_applicationName = applicationName;
		_hinstance = hinstance;
		_settings = settings;

		InitializeWindow(wndProc);
	}

	Window::~Window()
	{
		ShutdownWindow();
	}

	void Window::ShutdownWindow()
	{
		ShowCursor(true);

		SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);

		DestroyWindow(_hwnd);
		_hwnd = NULL;

		UnregisterClass(_applicationName, _hinstance);
		_hinstance = NULL;
	}

	void Window::InitializeWindow(WNDPROC wndProc)
	{
		_windowHandle = this;

		//Setup the windows class with default settings
		WNDCLASSEX wc;
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = wndProc;
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

		_defaultStyle = (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
		//_defaultStyle = GetWindowLong(_hwnd, GWL_STYLE);
		_defaultExStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);

		_hwnd = CreateWindowEx(WS_EX_APPWINDOW, _applicationName, _applicationName, _defaultStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, _hinstance, NULL);

		ResizeWindow(_settings);
	}

	bool Window::Run()
	{
		MSG msg;
		bool result = true;

		ZeroMemory(&msg, sizeof(MSG));

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			result = false;
		}
		return result;
	}

	void Window::ResizeWindow(System::Settings* settings)
	{
		_settings = settings;
		int systemWidth = GetSystemMetrics(SM_CXSCREEN);
		int systemHeight = GetSystemMetrics(SM_CYSCREEN);
		int windowWidth = _settings->_windowWidth;
		int windowHeight = _settings->_windowHeight;
		bool borderless = _settings->_borderless;

		LONG style = _defaultStyle;
		LONG exStyle = _defaultExStyle;

		//If settings is fullscreen, set the window to cover the entire screen and be borderless
		if (borderless)
		{
			style = _defaultStyle & ~(WS_CAPTION | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU | WS_POPUP | WS_BORDER);
			exStyle = _defaultExStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		}
		else //This is a window with borders - adjust the window size acordingly
		{
			RECT rect = { 0, 0, windowWidth, windowHeight };
			AdjustWindowRect(&rect, style, FALSE);
			windowWidth = rect.right - rect.left;
			windowHeight = rect.bottom - rect.top;
		}
		
		int windowPositionX = (systemWidth - windowWidth) / 2;
		int windowPositionY = (systemHeight - windowHeight) / 2;

		SetWindowLong(_hwnd, GWL_STYLE, style);
		SetWindowLong(_hwnd, GWL_EXSTYLE, exStyle);

		SetWindowPos(_hwnd, HWND_TOP, windowPositionX, windowPositionY, windowWidth, windowHeight, SWP_FRAMECHANGED);
		SetFocus(_hwnd);
		ShowCursor(_settings->_showMouseCursor);
		ShowWindow(_hwnd, SW_SHOW);
	}

	HWND Window::GetHWND()
	{
		return _hwnd;
	}	
}