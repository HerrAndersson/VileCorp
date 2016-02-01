#include "UITree.h"

using namespace rapidjson;
using namespace std;
using namespace DirectX;

namespace GUI
{
	UITree::UITree(const std::string& filename, const std::string& statename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height) : _info(fontWrapper, width, height)
	{
		_AM = assetManager;
		ifstream file(filename);
		if (!file.good())
		{
			throw std::runtime_error("Failed to open " + filename);
		}
		string str((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
		file.close();

		Document d;
		d.Parse(str.c_str());

		bool found = false;
		for (Value::ConstMemberIterator i = d.MemberBegin(); i != d.MemberEnd(); ++i)
		{
			if (i->name.IsString() && string(i->name.GetString()) == statename)
			{
				found = true;
				_root = LoadGUITree("root", i->value.MemberBegin(), i->value.MemberEnd());
			}
		}
		if (!found)
		{
			throw std::runtime_error("Could not find " + statename + " in " + filename);
		}
	}

	UITree::~UITree()
	{
		Release(_root);
	}

	Node * UITree::GetRootNode() const
	{
		return _root;
	}

	void UITree::Resize(int width, int height)
	{
		_info._screenWidth = width;
		_info._screenHeight = height;
		Resize(_root);
	}

	void UITree::Resize(Node* current)
	{
		for (Node* i : *current->GetChildren())
		{
			Resize(i);
		}
		current->UpdateFont();
	}

	Node* UITree::LoadGUITree(const std::string& name, rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end)
	{
		Node* returnNode = new Node(&_info);
		returnNode->SetId(name);
		for (Value::ConstMemberIterator i = start; i != end; ++i)
		{
			if (i->name == "position")
			{
				if (i->value.IsArray())
				{
					//if(test == "Trap" || returnNode->GetId() == "Unit" || returnNode->GetId() == "Entry" || returnNode->GetId() == "Floor" || returnNode->GetId() == "Wall" || returnNode->GetId() == "Decoration" || returnNode->GetId() == "Objective")
					returnNode->SetPosition(XMFLOAT2(
						(float)i->value[0].GetDouble(),
						(float)i->value[1].GetDouble()
						));
				}
			}
			else if (i->name == "scale")
			{
				if (i->value.IsArray())
				{
					returnNode->SetScale(XMFLOAT2(
						(float)i->value[0].GetDouble(),
						(float)i->value[1].GetDouble()
						));
				}
			}
			else if (i->name == "texture")
			{
				returnNode->SetTexture(_AM->GetTexture(i->value.GetString()));
			}
			else if (i->name == "text")
			{
				//Convert from utf-8 to wchar
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				returnNode->SetText(converter.from_bytes(i->value.GetString()));
			}
			else if (i->name == "color")
			{
				UINT color;
				std::stringstream ss;
				ss << std::hex << i->value.GetString();
				ss >> color;
				returnNode->SetColor(color);
			}
			else if (i->name == "fontsize")
			{
				returnNode->SetFontSize((float)i->value.GetDouble());
			}
			else if (i->name == "centered")
			{
				returnNode->SetCentered(i->value.GetBool());
			}
			else if (i->name == "hidden")
			{
				returnNode->SetHidden(i->value.GetBool());
			}
			else
			{
				returnNode->AddChild(LoadGUITree(i->name.GetString(), i->value.MemberBegin(), i->value.MemberEnd()));
			}
		}
		return returnNode;
	}

	void UITree::Release(Node * node)
	{
		for (Node* i : *node->GetChildren())
		{
			Release(i);
		}
		delete node;
	}

	bool UITree::IsButtonColliding(Node* current, const std::string& id, int x, int y, float px, float py, bool& found)
	{
		if (current->_id == id)
		{
			XMFLOAT2 topLeft;
			XMFLOAT2 size;

			XMFLOAT2 pos = current->GetPosition();
			XMFLOAT2 scale = current->GetScale();
			//(px, py) == center position
			//Calculate bouding box in pixels from scale and (px, py)
			topLeft.x = px - scale.x;
			topLeft.y = py*-1.0f - scale.y;

			//Convert coordinates to pixel coordinate system
			topLeft.x = (topLeft.x + 1.0f) * 0.5f * _info._screenWidth;
			topLeft.y = (topLeft.y + 1.0f) * 0.5f * _info._screenHeight;

			size.x = scale.x * _info._screenWidth;
			size.y = scale.y * _info._screenHeight;

			//Check collision with mouse coord and return the result
			found = true;
			return (
				(y > topLeft.y && y < topLeft.y + size.y) &&
				(x > topLeft.x && x < topLeft.x + size.x)
				);
		}
		for (Node* i : *current->GetChildren())
		{
			XMFLOAT2 pos = i->GetPosition();
			bool f;
			bool ret = IsButtonColliding(i, id, x, y, px + pos.x, py + pos.y, f);
			if (f)
			{
				found = true;
				return ret;
			}
		}
		found = false;
		return false;
	}

	bool UITree::IsButtonColliding(const std::string& id, int x, int y)
	{
		XMFLOAT2 pos = _root->GetPosition();
		bool f;
		return IsButtonColliding(_root, id, x, y, pos.x, pos.y, f);
	}

	bool UITree::IsButtonColliding(Node* current, int x, int y)
	{
		return IsButtonColliding(current->GetId(), x, y);
	}

	Node* UITree::FindNode(Node* current, const std::string& id)
	{
		if (current->GetId() == id)
		{
			return current;
		}
		for (Node* i : *current->GetChildren())
		{
			Node* node = FindNode(i, id);
			if (node != nullptr)
			{
				return node;
			}
		}
		return nullptr;
	}

	Node* UITree::GetNode(const std::string& id)
	{
		Node* node = _root;
		return FindNode(_root, id);
	}

	void UITree::ReloadTree(const std::string& filename, const std::string& statename)
	{
		Release(_root);
		ifstream file(filename);
		if (!file.good())
		{
			throw std::runtime_error("Failed to open " + filename);
		}
		string str((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
		file.close();

		Document d;
		d.Parse(str.c_str());

		bool found = false;
		for (Value::ConstMemberIterator i = d.MemberBegin(); i != d.MemberEnd(); ++i)
		{
			if (i->name.IsString() && string(i->name.GetString()) == statename)
			{
				found = true;
				_root = LoadGUITree("root", i->value.MemberBegin(), i->value.MemberEnd());
			}
		}
		if (!found)
		{
			throw std::runtime_error("Could not find " + statename + " in " + filename);
		}
	}
}
