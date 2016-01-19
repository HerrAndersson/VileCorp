#include "UITree.h"

using namespace rapidjson;
using namespace std;
using namespace DirectX;

namespace GUI
{
	UITree::UITree(const std::string& filename, const std::string& statename)
	{
		ifstream file(filename);
		if (!file.good())
		{
			throw std::runtime_error("Failed to open " + filename);
		}
		string str((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
		file.close();

		Document d;
		d.Parse(str.c_str());

		_root = new GUI::Node(XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));

		bool found = false;
		for (auto i = d.MemberBegin(); i != d.MemberEnd(); ++i)
		{
			if (i->name.IsString() &&  string(i->name.GetString()) == statename)
			{
				found = true;
				LoadGUITree(_root, i->value.MemberBegin(), i->value.MemberEnd());
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

	void UITree::LoadGUITree(Node * current, rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end)
	{
		Value::ConstMemberIterator it = start;
		for (; it != end; ++it)
		{
			string id(it->name.GetString());
			Node* newNode = nullptr;
			if (id == "_parent") //This is a transform node
			{
				XMFLOAT2 position;
				XMFLOAT2 scale;
				for (auto i = it->value.MemberBegin(); i != it->value.MemberEnd(); ++i)
				{
					if (i->name == "position")
					{
						if (i->value.IsArray())
						{
							position = XMFLOAT2((float)i->value[0].GetDouble(),
								(float)i->value[1].GetDouble());
						}
					}
					else if (i->name == "scale")
					{
						if (i->value.IsArray())
						{
							scale = XMFLOAT2((float)i->value[0].GetDouble(),
								(float)i->value[1].GetDouble());
						}
					}
				}
				newNode = new Node(position, scale);
			}
			else //This is an element
			{

			}

			current->GetChildren()->push_back(newNode);
		}
	}

	void UITree::Release(Node * node)
	{
		for (Node* i : *node->GetChildren())
		{
			Release(i);
		}
		if (node != nullptr)
		{
			delete node;
		}
	}
}
