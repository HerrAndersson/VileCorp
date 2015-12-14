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
			int mainKey;
			int keyModifier;
		};
		enum KeyModifiers
		{
			MODIFIER_NONE	= 0x0,
			MODIFIER_CTRL	= 0x1,
			MODIFIER_ALT	= 0x2,
			MODIFIER_SHIFT	= 0x4,
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