#include "Controls.h"

using namespace std;
using namespace rapidjson;

namespace System
{
	/*
	The controls class works by loading the controls.json file in the Asset folder.
	Keybind can only have on main key per function and any combination of modifiers

	Valid main keys are:
		A-Z (capital letters)
		0-9
		spacebar
		escape
		tab
		pageup
		pagedown
		end
		home
		leftarrow
		uparrow
		rightarrow
		downarrow
		delete
	Valid modifers are:
		ctrl
		alt
		shift
		repeat (if the should return true every time the key is checked)

	To use this, call "IsFunctionKeyDown" with the string defined as STATE:KEYBIND.
	Example:
		if (_controls->IsFunctionKeyDown("MAP_EDIT:SAVE"))
		{
			//Code
		}
		//Mattias
	*/
	Controls::Controls(System::InputDevice* input)
	{
		_keymap = new std::map<std::string, Key>();

		_inputDevice = input;
		ifstream file("Assets/controls.json");
		if (!file.good())
		{
			throw std::runtime_error("Failed to open controls.json");
		}
		string str((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
		file.close();

		Document d;
		d.Parse(str.c_str());

		//Loop through all the states
		for (Value::ConstMemberIterator it = d.MemberBegin(); it != d.MemberEnd(); ++it)
		{
			std::string currentStateName(it->name.GetString());
			char currentKeyMod = NONE;
			char mainKey = 0;

			//Loop through the whole keymap
			for (Value::ConstMemberIterator i = it->value.MemberBegin(); i != it->value.MemberEnd(); ++i)
			{
				string currentKeyMapName(i->name.GetString());
				const Value& currentKeyMap = it->value[currentKeyMapName.c_str()];

				if (currentKeyMap.IsArray())
				{
					for (SizeType j = 0; j < currentKeyMap.Size(); j++)
					{
						if (currentKeyMap[j].IsString())
						{
							string key(currentKeyMap[j].GetString());
							StringToKeyMap(key, mainKey, currentKeyMod);
						}
					}
					(*_keymap)[currentStateName + ":" + currentKeyMapName].keyModifier = currentKeyMod;
					(*_keymap)[currentStateName + ":" + currentKeyMapName].mainKey = mainKey;
				}

				if (!mainKey)
				{
					throw std::runtime_error("Failed to get keybind " + currentKeyMapName + " in controls.json");
				}
			}
		}
	}

	Controls::~Controls()
	{
		delete _keymap;
	}

	void Controls::StringToKeyMap(const std::string& key, char &mainKey, char& keyModifiers)
	{
		if (key == "ctrl")
		{
			keyModifiers |= CTRL;
		}
		else if (key == "alt")
		{
			keyModifiers |= ALT;
		}
		else if (key == "shift")
		{
			keyModifiers |= SHIFT;
		}
		else if (key == "repeat")
		{
			keyModifiers |= REPEAT;
		}
		else if (key == "spacebar")
		{
			mainKey = VK_SPACE;
		}
		else if (key == "escape")
		{
			mainKey = VK_ESCAPE;
		}
		else if (key == "tab")
		{
			mainKey = VK_TAB;
		}
		else if (key == "pageup")
		{
			mainKey = VK_PRIOR;
		}
		else if (key == "pagedown")
		{
			mainKey = VK_NEXT;
		}
		else if (key == "end")
		{
			mainKey = VK_END;
		}
		else if (key == "home")
		{
			mainKey = VK_HOME;
		}
		else if (key == "leftarrow")
		{
			mainKey = VK_LEFT;
		}
		else if (key == "uparrow")
		{
			mainKey = VK_UP;
		}
		else if (key == "rightarrow")
		{
			mainKey = VK_RIGHT;
		}
		else if (key == "downarrow")
		{
			mainKey = VK_DOWN;
		}
		else if (key == "delete")
		{
			mainKey = VK_DELETE;
		}
		else if (key.length() == 1) //Map the key directly to the ascii code
		{
			mainKey = key[0];
			if (!(mainKey >= '0' && mainKey <= '9' ||
				mainKey >= 'A' && mainKey <= 'Z'))
			{
				throw std::runtime_error("Invalid main key: \"" + key + "\"");
			}
		}
		else
		{
			throw std::runtime_error("Undefined keyword \"" + key + "\"");
		}
	}
	void Controls::ToggleCursorLock()
	{
		_inputDevice->ToggleCursorLock();
	}

	bool Controls::IsFunctionKeyDown(const std::string& key)
	{
		bool ret = false;
		int modifersActivated = NONE;
		if (_inputDevice->IsDown(Input::Shift))
		{
			modifersActivated |= SHIFT;
		}
		if (_inputDevice->IsDown(Input::Control))
		{
			modifersActivated |= CTRL;
		}
		if (_inputDevice->IsDown(Input::Alt))
		{
			modifersActivated |= ALT;
		}
		if ((*_keymap)[key].keyModifier & REPEAT)
		{
			ret = _inputDevice->IsDown((*_keymap)[key].mainKey);
			modifersActivated |= REPEAT;
		}
		else
		{
			ret = _inputDevice->IsPressed((*_keymap)[key].mainKey);
		}
		return ret && (*_keymap)[key].keyModifier == modifersActivated;
	}
}