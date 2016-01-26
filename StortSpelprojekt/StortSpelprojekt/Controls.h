#pragma once

#define SYSTEM_EXPORT __declspec(dllexport)

#include "InputDevice.h"
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"

#include <fstream>
#include <map>

namespace System
{
	enum KeyMapMode
	{
		MAP_EDIT_KEYMAP,
		PLACEMENT_KEYMAP,
		PLAY_KEYMAP,
		MENU_KEYMAP,
		DEBUG_KEYMAP
	};

	class SYSTEM_EXPORT Controls
	{
		struct Key
		{
			unsigned char mainKey;
			unsigned char keyModifier;
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
		std::string _allKeys;

		void StringToKeyMap(const std::string& key, unsigned char &mainKey, unsigned char& keyModifiers);
	public:
		Controls(HWND hwnd);
		~Controls();
		void Update();
		void HandleRawInput(LPARAM lparam);
		void SaveKeyBindings(int keyMap, std::string action, std::string newKey, std::string newKey2 = std::string(), std::string newKey3 = std::string(), std::string newKey4 = std::string());
		bool IsFunctionKeyDown(const std::string& key);
		bool IsFunctionKeyUp(const std::string& key);
		void ToggleCursorLock();

		bool CursorLocked();
		MouseCoord GetMouseCoord();
	};

}