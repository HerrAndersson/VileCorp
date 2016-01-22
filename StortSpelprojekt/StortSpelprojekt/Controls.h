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
		InputDevice* _inputDevice;
		std::map<std::string, Key>* _keymap;

		void StringToKeyMap(const std::string& key, char &mainKey, char& keyModifiers);
	public:
		Controls(System::InputDevice* input);
		~Controls();
		void ToggleCursorLock();

		bool IsFunctionKeyDown(const std::string& key);
		bool IsFunctionKeyUp(const std::string& key);

		MouseCoord getMouseCoord();
	};

}