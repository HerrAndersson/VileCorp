#include "Controls.h"
#include "rapidjson\writer.h"
#include "rapidjson\prettywriter.h"

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
		leftmouse
		rightmouse
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
	/*
	Text input

	Enter text input mode by calling SetIsTextInputMode. This method accepts a wstring for inital text,
	and boolean flags for breaking out of text edit mode: pressing escape, carriage return or tab.
	When in text edit mode, the normal input is blocked, except for the mouse buttons.
	to break out of text input mode manually, call ResetTextInputMode.
	Keep in mind the text input string is cleard by breaking out of text edit mode.

	Example:

	if (_controls->GetIsTextInputMode())
	{
		_uiTree.GetNode("text")->SetText(_controls->GetCurrentText());
	}
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		System::MouseCoord coord = _controls->GetMouseCoord();
		if (_uiTree.IsButtonColliding("text", coord._pos.x, coord._pos.y))
		{
			_controls->SetIsTextInputMode(_uiTree.GetNode("text")->GetText(), false, true);
		}
		else
		{
			_controls->ResetTextInputMode();
		}
	}
	//Mattias
	*/
	Controls::Controls(HWND hwnd)
	{
		_keymap = new std::map<std::string, Key>();

		_inputDevice = new InputDevice(hwnd);
		ifstream file("Assets/controls.json");
		if (!file.good())
		{
			throw std::runtime_error("Failed to open controls.json");
		}
		_allKeys = new string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
		file.close();

		Document d;

		d.Parse(_allKeys->c_str());

		//Loop through all the states
		for (Value::ConstMemberIterator it = d.MemberBegin(); it != d.MemberEnd(); ++it)
		{
			std::string currentStateName(it->name.GetString());

			//Loop through the whole keymap
			for (Value::ConstMemberIterator i = it->value.MemberBegin(); i != it->value.MemberEnd(); ++i)
			{
				unsigned char currentKeyMod = NONE;
				unsigned char mainKey = 0;

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
		delete _inputDevice;
		delete _allKeys;
	}

	void Controls::Update()
	{
		_inputDevice->Update();
	}

	void Controls::HandleRawInput(LPARAM lparam)
	{
		_inputDevice->HandleRawInput(lparam);
	}

	void Controls::HandleTextInput(WPARAM wparam, LPARAM lparam)
	{
		_inputDevice->HandleTextInput(wparam, lparam);
	}

	void Controls::ResetInputBuffers()
	{
		_inputDevice->ResetInputBuffers();
	}

	void Controls::StringToKeyMap(const std::string& key, unsigned char &mainKey, unsigned char& keyModifiers)
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
		else if (key == "enter" || key == "return")
		{
			mainKey = VK_RETURN;
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
		else if (key == "leftmouse")
		{
			mainKey = VK_LBUTTON;
		}
		else if (key == "rightmouse")
		{
			mainKey = VK_RBUTTON;
		}
		else if (key == "scrollup")
		{
			mainKey = (unsigned char)System::Input::ScrollWheelUp;
		}
		else if (key == "scrolldown")
		{
			mainKey = (unsigned char)System::Input::ScrollWheelDown;
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

	void Controls::SaveKeyBindings(int keyMap, const std::string& action, const std::string& newKey, const std::string& newKey2, const std::string& newKey3, const std::string& newKey4)
	{
		Document d;
		d.Parse(_allKeys->c_str());
		int currentKeyMap = 0;
		//Loop through all the states
		for (Value::MemberIterator it = d.MemberBegin(); it != d.MemberEnd(); ++it)
		{
			if (currentKeyMap == keyMap)
			{
				//Loop through the whole keymap
				for (Value::MemberIterator i = it->value.MemberBegin(); i != it->value.MemberEnd(); ++i)
				{
					std::string currentAction = i->name.GetString();
					if (currentAction == action)
					{
						i->value[0].SetString(newKey.c_str(), newKey.size(), d.GetAllocator());

						if (!newKey2.empty())
						{
							i->value[1].SetString(newKey2.c_str(), newKey2.size(), d.GetAllocator());
						}
						if (!newKey3.empty())
						{
							i->value[2].SetString(newKey3.c_str(), newKey3.size(), d.GetAllocator());
						}
						if (!newKey4.empty())
						{
							i->value[3].SetString(newKey4.c_str(), newKey4.size(), d.GetAllocator());
						}

						break;
					}
				}
				break;
			}
			currentKeyMap++;
		}

		//Temp write to temp file
		std::ofstream test("Assets/controlsTEST.json");
		
		//Uncomment to write to actual keybinding file.
		//std::ofstream test("Assets/controls.json");
		
		StringBuffer buffer;
		PrettyWriter<StringBuffer>writer(buffer);
		d.Accept(writer);

		std::string output = buffer.GetString();
		test.write(output.c_str(), buffer.GetSize());
		test.close();
	}

	bool Controls::IsFunctionKeyDown(const std::string& key)
	{
		bool ret = false;
		unsigned char modifersActivated = NONE;
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

	bool Controls::IsFunctionKeyUp(const std::string& key)
	{
		bool ret = false;
		unsigned char modifersActivated = NONE;
		if ((*_keymap)[key].keyModifier & REPEAT)
		{
			ret = _inputDevice->IsUp((*_keymap)[key].mainKey);
			modifersActivated |= REPEAT;
		}
		else
		{
			ret = _inputDevice->IsReleased((*_keymap)[key].mainKey);
		}
		return ret && (*_keymap)[key].keyModifier == modifersActivated;
	}

	void Controls::ToggleCursorLock()
	{
		_inputDevice->ToggleCursorLock();
	}

	bool Controls::CursorLocked()
	{
		return _inputDevice->CursorLocked();
	}

	MouseCoord Controls::GetMouseCoord()
	{
		return _inputDevice->GetMouseCoord();
	}

	MouseCoord Controls::GetClickedCoord()
	{
		return _inputDevice->GetClickedCoord();
	}

	void Controls::SetClickedCoord(const MouseCoord& pos)
	{
		_inputDevice->SetClickedCoord(pos);
	}

	void Controls::SetCurrentText(const std::wstring& text)
	{
		_inputDevice->SetCurrentText(text);
	}

	std::wstring Controls::GetCurrentText() const
	{
		return _inputDevice->GetCurrentText();
	}

	void Controls::SetIsTextInputMode(const std::wstring& currentText, bool breakOnEsc, bool breakOnCarriageReturn, bool breakOnTab, int characterLimit, bool onlyNumbers)
	{
		_inputDevice->SetIsTextInputMode(currentText, breakOnEsc, breakOnCarriageReturn, breakOnTab, characterLimit, onlyNumbers);
	}

	void Controls::ResetTextInputMode()
	{
		_inputDevice->ResetTextInputMode();
	}

	bool Controls::GetIsTextInputMode() const
	{
		return _inputDevice->GetIsTextInputMode();
	}

}