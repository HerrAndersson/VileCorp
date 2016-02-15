#include "UITree.h"

using namespace rapidjson;
using namespace std;
using namespace DirectX;

namespace GUI
{
	UITree::UITree(const std::string& filename, const std::string& statename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings) : _info(fontWrapper, settings->_screenWidth, settings->_screenHeight)
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
		_root->SetPosition(_root->GetFinalPosition());
	}

	UITree::~UITree()
	{
		Release(_root);
	}

	Node * UITree::GetRootNode() const
	{
		return _root;
	}

	void UITree::Resize(System::Settings* settings)
	{
		_info._screenWidth = settings->_screenWidth;
		_info._screenHeight = settings->_screenHeight;
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

	int UITree::CreateTilesetObject(Tileset::Object* object, Node* list, int index, int type, int subtype)
	{
		int createdThumbnails = 0;
		if (list == nullptr)
		{
			throw runtime_error("UITree::CreateTilesetObject: list not found");
		}
		for (unsigned i = 0; i < object->_thumbnails.size(); i++)
		{
			BlueprintNode* newNode = new BlueprintNode(&_info, type, subtype, i);
			int row = (index + createdThumbnails) % 28 / 4, column = (index + createdThumbnails) % 28 % 4, page = (index + createdThumbnails) / 28;

			newNode->SetId(object->_name);
			newNode->SetPosition(XMFLOAT2(-0.125 + (0.09 * column), 0.42 - (0.13 * row)));
			newNode->SetScale(XMFLOAT2(0.04, 0.060));
			newNode->SetTexture(_AM->GetTexture(object->_thumbnails[i])->_data);
			createdThumbnails++;
			if (page >= list->GetChildren()->size())
			{
				Node* newPage = new Node(&_info);
				newPage->SetId(list->GetId() + "page" + to_string(page));
				list->AddChild(newPage);
			}
			list->GetChildren()->at(page)->AddChild(newNode);
		}
		return createdThumbnails;
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
				returnNode->SetTexture(_AM->GetTexture(i->value.GetString())->_data);
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

	bool UITree::IsButtonColliding(Node* current, int x, int y)
	{

		XMFLOAT2 topLeft;
		XMFLOAT2 size;
		XMFLOAT2 pos = current->GetFinalPosition();
		XMFLOAT2 scale = current->GetScale();

		//(px, py) == center position
		//Calculate bouding box in pixels from scale and (px, py)
		topLeft.x = pos.x - scale.x;
		topLeft.y = pos.y*-1.0f - scale.y;

		//Convert coordinates to pixel coordinate system
		topLeft.x = (topLeft.x + 1.0f) * 0.5f * _info._screenWidth;
		topLeft.y = (topLeft.y + 1.0f) * 0.5f * _info._screenHeight;

		size.x = scale.x * _info._screenWidth;
		size.y = scale.y * _info._screenHeight;

		//Check collision with mouse coord and return the result
		return (
			(y > topLeft.y && y < topLeft.y + size.y) &&
			(x > topLeft.x && x < topLeft.x + size.x)
			);

		return false;
	}

	bool UITree::IsButtonColliding(const std::string& id, int x, int y)
	{
		return IsButtonColliding(UITree::GetNode(id), x, y);
//		return IsButtonColliding(current->GetId(), x, y);
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
		Node* node = FindNode(_root, id);
		if (node == nullptr)
		{
			throw runtime_error("UITree::GetNode error:\nNode " + id + " not found");
		}
		return node;
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
