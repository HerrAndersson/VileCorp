﻿#include "InputDevice.h"

#include <cwctype>

namespace System
{
	InputDevice::InputDevice(HWND hwnd)
	{
		_hwnd = hwnd;
		ResetInputBuffers();

		RECT rect;
		GetWindowRect(_hwnd, &rect);
		_mouseCoord._pos.x = (rect.left + (rect.right - rect.left)) / 2;
		_mouseCoord._pos.y = (rect.top + (rect.bottom - rect.top)) / 2;
		ScreenToClient(_hwnd, &_mouseCoord._pos);

		_mouseBuffer._pos.x = _mouseCoord._pos.x;
		_mouseBuffer._pos.y = _mouseCoord._pos.y;
		_mouseBuffer._deltaPos.x = 0;
		_mouseBuffer._deltaPos.y = 0;

		_rawBufferSize = 256;
		_rawBuffer = new BYTE[_rawBufferSize];
		RegisterDevice(hwnd);

		_lockedCursor = false;

		_currentText.clear();
		_isTextInputMode = false;
		_breakOnEsc = false;
		_breakOnCarriageReturn = false;
		_breakOnTab = false;
		_mouseClicked = { 0 };
		_characterLimit = 0;
		_onlyNumbers = false;
	}
	InputDevice::~InputDevice()
	{
		delete[]_rawBuffer;
	}

	bool InputDevice::IsMouseKey(int key)
	{
		return (
				key == Input::LeftMouse ||
				key == Input::RightMouse ||
				key == Input::Mouse3 ||
				key == Input::Mouse4 ||
				key == Input::Mouse5
			);
	}

	/*
	The bool in the update is for locking mouse,
	needed only when using free cam
	*/
	void InputDevice::Update()
	{
		memcpy(_last, _current, sizeof(bool) * KEY_CODE_CAP);
		memcpy(_current, _buffer, sizeof(bool) * KEY_CODE_CAP);

		//Reset the scroll wheel state every frame
		_buffer[Input::ScrollWheelDown] = false;
		_buffer[Input::ScrollWheelUp] = false;


		if (!_lockedCursor)
		{
			//Add the delta pos to absulute pos
			_mouseCoord._deltaPos.x = _mouseBuffer._deltaPos.x;
			_mouseCoord._deltaPos.y = _mouseBuffer._deltaPos.y;

			_mouseBuffer._deltaPos.x = 0;
			_mouseBuffer._deltaPos.y = 0;

			_mouseCoord._pos.x += _mouseCoord._deltaPos.x;
			_mouseCoord._pos.y += _mouseCoord._deltaPos.y;
			//SetCursorPos(_mouseCoord._pos.x, _mouseCoord._pos.y);
		}
		else
		{
			RECT rect;
			GetWindowRect(_hwnd, &rect);

			GetCursorPos(&_mouseCoord._pos);
			_mouseCoord._deltaPos.x = _mouseCoord._pos.x - (rect.left + (rect.right - rect.left) / 2);
			_mouseCoord._deltaPos.y = _mouseCoord._pos.y - (rect.top + (rect.bottom - rect.top) / 2);

			if (GetFocus() == _hwnd)
			{
				//Sets mouse position to the middle of the window
				SetCursorPos(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
				ClipCursor(&rect);
			}
		}
		GetCursorPos(&_mouseCoord._pos);
		ScreenToClient(_hwnd, &_mouseCoord._pos);
	}

	void InputDevice::ResetInputBuffers()
	{
		for (int i = 0; i < KEY_CODE_CAP; i++)
		{
			_current[i] = false;
			_last[i] = false;
			_buffer[i] = false;
		}
	}

	void InputDevice::ToggleCursorLock()
	{
		if (_lockedCursor)
		{
			_lockedCursor = false;
			while (ShowCursor(true) <= 0)
			{
				ShowCursor(true);
			}
		}
		else
		{
			_lockedCursor = true;
			while (ShowCursor(false) >= 0)
			{
				ShowCursor(false);
			}
		}
	}

	bool InputDevice::CursorLocked()
	{
		return _lockedCursor;
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


		//Mouse movement, scroll and mouse buttons
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
				SHORT mouseDelta = (SHORT)raw->data.mouse.usButtonData;

				if (mouseDelta > 0)
				{
					_buffer[Input::ScrollWheelUp] = true;
				}
				else if (mouseDelta < 0)
				{
					_buffer[Input::ScrollWheelDown] = true;
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
			int flags = raw->data.keyboard.Flags;

			if (flags == RI_KEY_MAKE || flags == RI_KEY_MAKE + RI_KEY_E0)
			{
				_buffer[raw->data.keyboard.VKey] = true;
			}
			if (flags == RI_KEY_BREAK || flags == RI_KEY_BREAK + RI_KEY_E0)
			{
				_buffer[raw->data.keyboard.VKey] = false;
			}
		}

		//clean up of buffer
		DefRawInputProc((PRAWINPUT*)_rawBuffer, 1, sizeof(RAWINPUTHEADER));
	}

	void InputDevice::HandleTextInput(WPARAM wparam, LPARAM lparam)
	{
		if (_isTextInputMode)
		{
			bool overCharacterLimit = false;
			if ((int)_currentText.size() >= _characterLimit)
			{
				overCharacterLimit = true;
			}

			switch (wparam)
			{
			case 0x08: //Process a backspace
			{
				if (_currentText.size() > 0)
				{
					_currentText.pop_back();
				}
				break;
			}
			case 0x1B: //Process an escape
			{
				if (_breakOnEsc)
				{
					ResetTextInputMode();
				}
				break;
			}
			case 0x09: //Process a tab
			{
				if (_breakOnTab)
				{
					ResetTextInputMode();
				}
				else
				{
					if (!overCharacterLimit)
					{
						_currentText += wparam;
					}
				}
				break;
			}
			case 0x0D: //Process a carriage return
			{
				if (_breakOnCarriageReturn)
				{
					ResetTextInputMode();
				}
				else
				{
					if (!overCharacterLimit)
					{
						_currentText += wparam;
					}
				}
				break;
			}
			default: //Process displayable characters
			{

				if (iswprint(wparam))
				{
					if (wparam <= 0x39 && wparam >= 0x30 || _onlyNumbers== false)
					{
						
						if (!overCharacterLimit)
						{
							_currentText += wparam;
						}
					}
				}
				break;
			}
			}
		}
	}

	void InputDevice::RegisterDevice(HWND hwnd)
	{
		const int numberOfDevices = 2;
		RAWINPUTDEVICE rid[numberOfDevices];

		rid[0].dwFlags = 0;//RIDEV_CAPTUREMOUSE
		rid[0].hwndTarget = hwnd;
		rid[0].usUsage = 0x02; //0x02 == MOUSE
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
		return (IsMouseKey(key) || !_isTextInputMode) && _current[key];
	}

	bool InputDevice::IsUp(int key)
	{
		return (IsMouseKey(key) || !_isTextInputMode) && !_current[key];
	}

	bool InputDevice::IsPressed(int key)
	{
		return (IsMouseKey(key) || !_isTextInputMode) && _current[key] && !_last[key];
	}

	bool InputDevice::IsReleased(int key)
	{
		return (IsMouseKey(key) || !_isTextInputMode) && !_current[key] && _last[key];
	}

	MouseCoord InputDevice::GetMouseCoord()const
	{
		return _mouseCoord;
	}

	void InputDevice::SetClickedCoord(const MouseCoord& pos)
	{
		_mouseClicked = pos;
	}

	MouseCoord InputDevice::GetClickedCoord()const
	{
		return _mouseClicked;
	}

	void InputDevice::SetCurrentText(const std::wstring& text)
	{
		_currentText = text;
	}

	std::wstring InputDevice::GetCurrentText() const
	{
		return _currentText;
	}

	void InputDevice::SetIsTextInputMode(const std::wstring& currentText, bool breakOnEsc, bool breakOnCarriageReturn, bool breakOnTab, int characterLimit, bool onlyNumbers)
	{
		_isTextInputMode = true;
		_currentText = currentText;
		_breakOnEsc = breakOnEsc;
		_breakOnCarriageReturn = breakOnCarriageReturn;
		_breakOnTab = breakOnTab;
		_characterLimit = characterLimit;
		_onlyNumbers = onlyNumbers;
	}
	void InputDevice::ResetTextInputMode()
	{
		_isTextInputMode = false;
		_currentText.clear();
	}

	bool InputDevice::GetIsTextInputMode() const
	{
		return _isTextInputMode;
	}

}