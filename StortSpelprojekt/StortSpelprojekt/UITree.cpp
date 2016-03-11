#include "UITree.h"

using namespace rapidjson;
using namespace std;
using namespace DirectX;

namespace GUI
{
	UITree::UITree(const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader) :
		_info(fontWrapper,
			settingsReader->GetSettings()->_screenWidth, settingsReader->GetSettings()->_screenHeight,
			settingsReader->GetSettings()->_windowWidth, settingsReader->GetSettings()->_windowHeight,
			  settingsReader->GetSettings()->_screenMode)
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

		_root = LoadGUITree("root", d.MemberBegin(), d.MemberEnd());
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
		_info._windowWidth = settings->_windowWidth;
		_info._windowHeight = settings->_windowHeight;
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

	int UITree::CreateBlueprintNodes(System::Blueprint* object, Node* list)
	{
		int createdThumbnails = 0;

		if (object && list)
		{
			std::vector<GUI::Node*>* pages = list->GetChildren();
			int index = 0;
			if (pages->size() > 0)
			{
				for (GUI::Node* pageNode : *pages)
				{
					index += pageNode->GetChildren()->size();
				}
			}

			if (list == nullptr)
			{
				throw runtime_error("UITree::CreateBlueprintNodes: list not found");
			}
			for (unsigned i = 0; i < object->_thumbnails.size(); i++)
			{
				BlueprintNode* newNode = new BlueprintNode(&_info, object, i);
				int row = (index + createdThumbnails) % 28 * 0.25f;
				int column = (index + createdThumbnails) % 4;  //Used to be % 28 % 4, removed % 28 since it always is the same as % 4
				int page = (index + createdThumbnails) / 28;

				newNode->SetId(object->_name);
				newNode->SetPosition(XMFLOAT2(-0.125f + (0.09f * column), 0.42f - (0.13f * row)));
				newNode->SetScale(XMFLOAT2(0.04f, 0.06f));
				newNode->SetTexture(_AM->GetTexture(object->_thumbnails[i]));
				createdThumbnails++;
				if (page >= (int)list->GetChildren()->size())
				{
					Node* newPage = new Node(&_info);
					newPage->SetId(list->GetId() + "page" + to_string(page));
					list->AddChild(newPage);
				}
				list->GetChildren()->at(page)->AddChild(newNode);
			}
		}
		return createdThumbnails;
	}
	
	Node* UITree::LoadGUITree(const std::string& name, rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end, Node* parent)
	{
		if (parent != nullptr && parent->GetId() == "UnitList")
		{
			return ParseBlueprintNode(start, end, parent, name);
		}
		return ParseRegularNode(start, end, parent, name);
	}

	Node* UITree::ParseRegularNode(rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end, Node* parent, const std::string& name)
	{
		Node* returnNode = new Node(&_info);
		returnNode->SetParent(parent);
		returnNode->SetId(name);
		for (Value::ConstMemberIterator i = start; i != end; ++i)
		{
			if (RegularNodeDataParsed(i, returnNode))
			{
				returnNode->AddChild(LoadGUITree(i->name.GetString(), i->value.MemberBegin(), i->value.MemberEnd(), returnNode));
			}
		}
		return returnNode;
	}

	bool UITree::RegularNodeDataParsed(rapidjson::Value::ConstMemberIterator i, Node* returnNode)
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
			return true;
		}
		return false;
	}

	BlueprintNode* UITree::ParseBlueprintNode(rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end, Node* parent, const std::string& name)
	{
		BlueprintNode* returnNode = new BlueprintNode(&_info);
		returnNode->SetParent(parent);
		returnNode->SetId(name);
		for (Value::ConstMemberIterator i = start; i != end; ++i)
		{
			if (RegularNodeDataParsed(i, returnNode) && BlueprintNodeDataParsed(i, returnNode))
			{
				returnNode->AddChild(LoadGUITree(i->name.GetString(), i->value.MemberBegin(), i->value.MemberEnd(), returnNode));
			}
		}
		return returnNode;
	}

	bool UITree::BlueprintNodeDataParsed(rapidjson::Value::ConstMemberIterator i, BlueprintNode* returnNode)
	{
		if (i->name == "type")
		{
			returnNode->SetType(i->value.GetInt());
		}
		else if (i->name == "subType")
		{
			returnNode->SetSubType(i->value.GetInt());
		}
		else if (i->name == "textureId")
		{
			returnNode->SetTextureId(i->value.GetInt());
		}
		else
		{
			return true;
		}
		return false;
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

		int width = _info._windowWidth;
		int height = _info._windowHeight;

		//Convert coordinates to pixel coordinate system
		topLeft.x = (topLeft.x + 1.0f) * 0.5f * width;
		topLeft.y = (topLeft.y + 1.0f) * 0.5f * height;

		size.x = scale.x * width;
		size.y = scale.y * height;

		//Check collision with mouse coord and return the result

		return (
			(y > topLeft.y && y < topLeft.y + size.y) &&
			(x > topLeft.x && x < topLeft.x + size.x)
			);
	}

	bool UITree::IsButtonColliding(const std::string& id, int x, int y)
	{
		return IsButtonColliding(UITree::GetNode(id), x, y);
	}

	bool UITree::IsButtonColliding(const std::string& id, const System::MouseCoord& coord)
	{
		return IsButtonColliding(id, coord._pos.x, coord._pos.y);
	}

	bool UITree::IsButtonColliding(Node* current, const System::MouseCoord& coord)
	{
		return IsButtonColliding(current, coord._pos.x, coord._pos.y);
	}

	bool UITree::IsNodeHidden(const std::string& id)
	{
		Node* node = GetNode(id);
		return IsNodeHidden(node);
	}

	bool UITree::IsNodeHidden(Node* node)
	{
		bool isHidden = false;
		if (node != nullptr)
		{
			while (node->GetParent() != nullptr && !isHidden)
			{
				node = node->GetParent();
				isHidden = node->GetHidden();
			}
		}
		else
		{
			isHidden = true;
		}
		return isHidden;
	}

	void UITree::HideNodeAndChildren(GUI::Node* node)
	{
		node->SetHidden(true);
		std::vector<Node*>* children = node->GetChildren();
		for (int i = 0; i < (int)children->size(); i++)
		{
			HideNodeAndChildren(children->at(i));
		}
	}

	void UITree::ShowNodeAndParents(GUI::Node* node)
	{
		node->SetHidden(false);
		Node* parent = node->GetParent();
		if (parent != nullptr)
		{
			ShowNodeAndParents(parent);
		}
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

	void UITree::ReloadTree(const std::string& filename)
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

		_root = LoadGUITree("root", d.MemberBegin(), d.MemberEnd());
		_root->SetPosition(_root->GetFinalPosition());
	}
}
