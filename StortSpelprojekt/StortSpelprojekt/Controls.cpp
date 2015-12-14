#include "Controls.h"

using namespace std;
using namespace rapidjson;

namespace System
{
	Controls::Controls(HWND hwnd) : _inputDevice(hwnd)
	{
		ifstream file("../../Output/Bin/x86/Debug/Assets/controls.json");
		if (!file.good())
		{
			throw std::runtime_error("Failed to open controls.json");
		}
		string str((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
		file.close();

		Document d;
		d.Parse(str.c_str());

		for (Value::ConstMemberIterator it = d.MemberBegin(); it != d.MemberEnd(); ++it)
		{
			string currentKeyMapName(it->name.GetString());
			const Value& currentKeyMap = d[currentKeyMapName.c_str()];
			int currentKeyMod = MODIFIER_NONE;
			int mainKey = 0;

			if (currentKeyMap.IsArray())
			{
				for (SizeType j = 0; j < currentKeyMap.Size(); j++)
				{
					if (currentKeyMap[j].IsString())
					{
						string key(currentKeyMap[j].GetString());
						if (key == "ctrl")
						{
							currentKeyMod |= MODIFIER_CTRL;
						}
						else if (key == "alt")
						{
							currentKeyMod |= MODIFIER_ALT;
						}
						else if (key == "shift")
						{
							currentKeyMod |= MODIFIER_SHIFT;
						}
						else if (key.length() == 1) //This is a single char, i.e. the main key
						{
							mainKey = (int)key[0];
						}
					}
				}
				keymap[currentKeyMapName].keyModifier = currentKeyMod;
				keymap[currentKeyMapName].mainKey = mainKey;
			}
			if (!mainKey)
			{
				throw std::runtime_error("Failed to get keybind " + currentKeyMapName + " in controls.json");
			}
		}
	}

	Controls::~Controls()
	{
	}

	void Controls::Update()
	{
		_inputDevice.Update();
	}
	void Controls::ToggleCursorLock()
	{
		_inputDevice.ToggleCursorLock();
	}

	bool Controls::IsFunctionKeyDown(const std::string& key)
	{
		bool ret = false;
		int modifersActivated = MODIFIER_NONE;
		if (_inputDevice.IsDown(Input::Shift))
		{
			modifersActivated |= MODIFIER_SHIFT;
		}
		if (_inputDevice.IsDown(Input::Control))
		{
			modifersActivated |= MODIFIER_CTRL;
		}
		if (_inputDevice.IsDown(Input::Alt))
		{
			modifersActivated |= MODIFIER_ALT;
		}
		return _inputDevice.IsPressed(keymap[key].mainKey) && keymap[key].keyModifier == modifersActivated;
	}


	bool System::Controls::IsMouseFunctionKeyDown(const std::string & key, System::MouseCoord & position)
	{
		return false;
	}
}