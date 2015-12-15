#pragma once

#define SYSTEM_EXPORT __declspec(dllexport)

#include "InputDevice.h"
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"

#include <fstream>
#include <map>

namespace System
{
	class SYSTEM_EXPORT Controls
	{
		struct Key
		{
			char mainKey;
			char keyModifier;
		};
		enum KeyModifiers
		{
			NONE	= 0x0,
			CTRL	= 0x1,
			ALT		= 0x2,
			SHIFT	= 0x4,
			REPEAT	= 0x8
		};

	private:
		InputDevice _inputDevice;
		std::map<std::string, Key> keymap;
	public:
		Controls(HWND hwnd);
		~Controls();
		void Update();
		void ToggleCursorLock();

		bool IsFunctionKeyDown(const std::string& key);
		bool IsMouseFunctionKeyDown(const std::string& key, System::MouseCoord& position);
	};

}