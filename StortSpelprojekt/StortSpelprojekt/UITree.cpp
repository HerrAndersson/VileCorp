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
			if (i->name.IsString() &&  string(i->name.GetString()) == statename)
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
}