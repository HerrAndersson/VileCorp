#pragma once

#define SYSTEM_EXPORT __declspec(dllexport)

#include "InputDevice.h"
#include "FunctionKey.h"

namespace System
{
	class SYSTEM_EXPORT Controls
	{
	private:
		InputDevice _inputDevice;
	public:
		Controls(HWND hwnd);
		~Controls();
		void Update();
		void ToggleCursorLock();

		bool IsFunctionKeyDown(FunctionKey key);
		bool IsMouseFunctionKeyDown(FunctionKey key, System::MouseCoord& position);
	};

}