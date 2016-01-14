#include "InputDevice.h"

namespace System
{
	InputDevice::InputDevice(HWND hwnd)
	{
		_hwnd = hwnd;
		for (int i = 0; i < KEY_CODE_CAP; i++)
		{
			_current[i] = false;
			_last[i] = false;
			_buffer[i] = false;
		}

		RECT rect;
		GetWindowRect(_hwnd, &rect);
		_mouseCoord._pos.x = (rect.left + (rect.right - rect.left)) / 2;
		_mouseCoord._pos.y = (rect.top + (rect.bottom - rect.top)) / 2;
		SetCursorPos(_mouseCoord._pos.x, _mouseCoord._pos.y);

		_mouseBuffer._pos.x = _mouseCoord._pos.x;
		_mouseBuffer._pos.y = _mouseCoord._pos.y;
		_mouseBuffer._deltaPos.x = 0;
		_mouseBuffer._deltaPos.y = 0;
		
		_rawBufferSize = 256;
		_rawBuffer = new BYTE[_rawBufferSize];
		RegisterDevice(hwnd);
	}
	InputDevice::~InputDevice()
	{
		if (_rawBuffer != nullptr) {
			delete []_rawBuffer;
			_rawBuffer = nullptr;
		}
	}

	void InputDevice::Update()
	{
		memcpy(_last, _current, sizeof(bool) * KEY_CODE_CAP);
		memcpy(_current, _buffer, sizeof(bool) * KEY_CODE_CAP);

		_mouseCoord._deltaPos.x = _mouseBuffer._deltaPos.x;
		_mouseCoord._deltaPos.y = _mouseBuffer._deltaPos.y;
		
		_mouseBuffer._deltaPos.x -= _mouseCoord._deltaPos.x;
		_mouseBuffer._deltaPos.y -= _mouseCoord._deltaPos.y;

		_mouseCoord._pos.x += _mouseCoord._deltaPos.x;
		_mouseCoord._pos.y += _mouseCoord._deltaPos.y;
		SetCursorPos(_mouseCoord._pos.x, _mouseCoord._pos.y);
	}

	void InputDevice::HandleRawInput(LPARAM lParam)
	{
		UINT rawBufferSize = 0;
		
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &rawBufferSize, sizeof(RAWINPUTHEADER));

		if (rawBufferSize > _rawBufferSize)
		{
			delete[] _rawBuffer;
			_rawBuffer = new BYTE[rawBufferSize];
		}
		
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, _rawBuffer, &rawBufferSize, sizeof(RAWINPUTHEADER));


		RAWINPUT* raw = (RAWINPUT*)_rawBuffer;


		//MOUSE
		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			switch (raw->data.mouse.usButtonFlags)
			{
			case RI_MOUSE_LEFT_BUTTON_DOWN:
			{
				_buffer[Input::LeftMouse] = true;
				break;
			}
			case RI_MOUSE_LEFT_BUTTON_UP:
			{
				_buffer[Input::LeftMouse] = false;
				break;
			}
			case RI_MOUSE_RIGHT_BUTTON_DOWN:
			{
				_buffer[Input::RightMouse] = true;
				break;
			}
			case RI_MOUSE_RIGHT_BUTTON_UP:
			{
				_buffer[Input::RightMouse] = false;
				break;
			}
			case RI_MOUSE_WHEEL:
			{
				if ((signed)raw->data.mouse.usButtonData < 0)
				{
					_buffer[Input::ScrollWheelUp] = true;
				}
				else if (raw->data.mouse.usButtonData > 0)
				{
					_buffer[Input::ScrollWheelDown] = true;
				}
				else if (raw->data.mouse.usButtonData == 0)
				{
					_buffer[Input::ScrollWheelDown] = false;
					_buffer[Input::ScrollWheelUp] = false;
				}
				break;
			}
			case RI_MOUSE_BUTTON_3_DOWN:
			{
				_buffer[Input::Mouse3] = true;
				break;
			}
			case RI_MOUSE_BUTTON_3_UP:
			{
				_buffer[Input::Mouse3] = false;
				break;
			}
			case RI_MOUSE_BUTTON_4_DOWN:
			{
				_buffer[Input::Mouse4] = true;
				break;
			}
			case RI_MOUSE_BUTTON_4_UP:
			{
				_buffer[Input::Mouse4] = false;
				break;
			}
			case RI_MOUSE_BUTTON_5_DOWN:
			{
				_buffer[Input::Mouse5] = true;
				break;
			}
			case RI_MOUSE_BUTTON_5_UP:
			{
				_buffer[Input::Mouse5] = false;
				break;
			}
			case MOUSE_MOVE_RELATIVE:
			{
				_mouseBuffer._deltaPos.x += raw->data.mouse.lLastX;
				_mouseBuffer._deltaPos.y += raw->data.mouse.lLastY;
				break;
			}
			}

		}

		//KEYBOARD
		if (raw->header.dwType == RIM_TYPEKEYBOARD)
		{
			if (raw->data.keyboard.Flags == RI_KEY_MAKE)
			{
				_buffer[raw->data.keyboard.VKey] = true;
			}
			if (raw->data.keyboard.Flags == RI_KEY_BREAK)
			{
				_buffer[raw->data.keyboard.VKey] = false;
			}

			// Provides the left/right version of a key.
			// Not used in our project and skipped.
			/*
			switch (raw->data.keyboard.Flags == RI_KEY_E0) //Left version of a key
			{}
			switch (raw->data.keyboard.Flags == RI_KEY_E1) //Right version of a key
			{}
			*/
		}
		
	
	
		//clean up of buffer
		DefRawInputProc((PRAWINPUT*)_rawBuffer, 1, sizeof(RAWINPUTHEADER));
	}

	void InputDevice::RegisterDevice(HWND hwnd)
	{
		const int numberOfDevices = 2;
		RAWINPUTDEVICE rid[numberOfDevices];

		rid[0].dwFlags = 0;//RIDEV_CAPTUREMOUSE
		rid[0].hwndTarget = hwnd;
		rid[0].usUsage = 0x02 ; //0x02 == MOUSE
		rid[0].usUsagePage = 0x01;

		rid[1].dwFlags = 0;
		rid[1].hwndTarget = hwnd;
		rid[1].usUsage = 0x06; //0x06 == KEYBOARD
		rid[1].usUsagePage = 0x01;

		bool result = true;
		UINT result1 = RegisterRawInputDevices(rid, numberOfDevices, sizeof(rid[0]));
		if (!result)
		{
			throw std::runtime_error("Error in RegisterRawInputDevices");
		}
	}

	bool InputDevice::IsDown(int key)
	{
		return _current[key];
	}

	bool InputDevice::IsUp(int key)
	{
		return !_current[key];
	}

	bool InputDevice::IsPressed(int key)
	{
		return _current[key] && !_last[key];
	}

	bool InputDevice::IsReleased(int key)
	{
		return !_current[key] && _last[key];
	}

	MouseCoord InputDevice::GetMouseCoord()const
	{
		return _mouseCoord;
	}
}