#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#define SYSTEM_EXPORT __declspec(dllexport)
#include <Windows.h>
#include <stdexcept>

namespace System
{
	struct MouseCoord
	{
		POINT _pos;
		POINT _deltaPos;
	};

	class SYSTEM_EXPORT InputDevice
	{
	private:
		MouseCoord	_mouseCoord;
		MouseCoord	_mouseBuffer;

		//142 due to 140 keycodes + 0 is skipped.
		const static int KEY_CODE_CAP = 142;
		bool _current[KEY_CODE_CAP];
		bool _last[KEY_CODE_CAP];
		bool _buffer[KEY_CODE_CAP];

		//Raw input Data
		BYTE* _rawBuffer;
		UINT _rawBufferSize;

		HWND _hwnd;

	public:
		InputDevice(HWND hwnd);
		~InputDevice();

		void Update(bool lockMouse = false);

		void HandleRawInput(LPARAM lparam);
		void RegisterDevice(HWND hwnd);
		
		bool IsDown(int key);
		bool IsUp(int key);
		bool IsPressed(int key);
		bool IsReleased(int key);
		MouseCoord GetMouseCoord()const;
	};

	namespace Input
	{
		const int LeftMouse = VK_LBUTTON;
		const int RightMouse = VK_RBUTTON;
		const int CANCEL = VK_CANCEL;
		const int MiddleMouse = VK_MBUTTON;
		const int X1Mouse = VK_XBUTTON1;
		const int X2Mouse = VK_XBUTTON2;
		const int Backspace = VK_BACK;
		const int Tab = VK_TAB;
		const int Clear = VK_CLEAR;
		const int Enter = VK_RETURN;
		const int Shift = VK_SHIFT;
		const int Control = VK_CONTROL;
		const int Alt = VK_MENU;
		const int Pause = VK_PAUSE;
		const int CapsLock = VK_CAPITAL;
		const int IME_KANA = VK_KANA;
		const int IME_HANGUL = VK_HANGUL;
		const int IME_JUNJA = VK_JUNJA;
		const int IME_FINAL = VK_FINAL;
		const int IME_HANJA = VK_HANJA;
		const int IME_KANJI = VK_KANJI;
		const int Escape = VK_ESCAPE;
		const int IME_CONVERT = VK_CONVERT;
		const int IME_NONCONVERT = VK_NONCONVERT;
		const int IME_ACCEPT = VK_ACCEPT;
		const int IME_MODECHANGE = VK_MODECHANGE;
		const int Spacebar = VK_SPACE;
		const int PageUp = VK_PRIOR;
		const int PageDown = VK_NEXT;
		const int End = VK_END;
		const int Home = VK_HOME;
		const int LeftArrow = VK_LEFT;
		const int UpArrow = VK_UP;
		const int RightArrow = VK_RIGHT;
		const int DownArrow = VK_DOWN;
		const int Select = VK_SELECT;
		const int Print = VK_PRINT;
		const int Execute = VK_EXECUTE;
		const int PrintScreen = VK_SNAPSHOT;
		const int Insert = VK_INSERT;
		const int Delete = VK_DELETE;
		const int Help = VK_HELP;

		const int Keyboard0 = 0x30;
		const int Keyboard1 = 0x31;
		const int Keyboard2 = 0x32;
		const int Keyboard3 = 0x33;
		const int Keyboard4 = 0x34;
		const int Keyboard5 = 0x35;
		const int Keyboard6 = 0x36;
		const int Keyboard7 = 0x37;
		const int Keyboard8 = 0x38;
		const int Keyboard9 = 0x39;

		const int A = 0x41;
		const int B = 0x42;
		const int C = 0x43;
		const int D = 0x44;
		const int E = 0x45;
		const int F = 0x46;
		const int G = 0x47;
		const int H = 0x48;
		const int I = 0x49;
		const int J = 0x4A;
		const int K = 0x4B;
		const int L = 0x4C;
		const int M = 0x4D;
		const int N = 0x4E;
		const int O = 0x4F;
		const int P = 0x50;
		const int Q = 0x51;
		const int R = 0x52;
		const int S = 0x53;
		const int T = 0x54;
		const int U = 0x55;
		const int V = 0x56;
		const int W = 0x57;
		const int X = 0x58;
		const int Y = 0x59;
		const int Z = 0x5A;

		const int LeftWinKey = VK_LWIN;
		const int RightWinKey = VK_RWIN;
		const int Sleep = VK_SLEEP;

		const int Numpad0 = VK_NUMPAD0;
		const int Numpad1 = VK_NUMPAD1;
		const int Numpad2 = VK_NUMPAD2;
		const int Numpad3 = VK_NUMPAD3;
		const int Numpad4 = VK_NUMPAD4;
		const int Numpad5 = VK_NUMPAD5;
		const int Numpad6 = VK_NUMPAD6;
		const int Numpad7 = VK_NUMPAD7;
		const int Numpad8 = VK_NUMPAD8;
		const int Numpad9 = VK_NUMPAD9;
		const int NumpadMul = VK_MULTIPLY;
		const int NumpadAdd = VK_ADD;
		const int Separator = VK_SEPARATOR;
		const int Subtract = VK_SUBTRACT;
		const int Decimal = VK_DECIMAL;
		const int Divide = VK_DIVIDE;

		const int F1 = VK_F1;
		const int F2 = VK_F2;
		const int F3 = VK_F3;
		const int F4 = VK_F4;
		const int F5 = VK_F5;
		const int F6 = VK_F6;
		const int F7 = VK_F7;
		const int F8 = VK_F8;
		const int F9 = VK_F9;
		const int F10 = VK_F10;
		const int F11 = VK_F11;
		const int F12 = VK_F12;
		const int F13 = VK_F13;
		const int F14 = VK_F14;
		const int F15 = VK_F15;
		const int F16 = VK_F16;
		const int F17 = VK_F17;
		const int F18 = VK_F18;
		const int F19 = VK_F19;
		const int F20 = VK_F20;
		const int F21 = VK_F21;
		const int F22 = VK_F22;
		const int F23 = VK_F23;
		const int F24 = VK_F24; //135

		const int ScrollWheelUp		= 136;
		const int ScrollWheelDown	= 137;
		const int Mouse3			= 138;
		const int Mouse4			= 139;
		const int Mouse5			= 140;
	}
	
}

#endif