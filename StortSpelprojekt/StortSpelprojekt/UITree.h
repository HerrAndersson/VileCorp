#pragma once
#include "Node.h"
#include "BlueprintNode.h"
#include "AssetManager.h"
#include "ObjectHandler.h"
#include "SettingsReader.h"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>
#include <locale>
#include <codecvt>
#include <string>
#include <sstream>

namespace GUI
{
	class UITree
	{
	private:
		Node* _root;
		AssetManager* _AM;
		Node::NodeInfo _info;
	private:
		Node* LoadGUITree(const std::string& name, rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end);
		void Release(Node* node);
		Node* FindNode(Node* current, const std::string& id);
	public:
		UITree(const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader);
		~UITree();

		GUI::Node* GetRootNode() const;
		void Resize(System::Settings* settings);
		void Resize(Node* current);

		int CreateTilesetObject(Blueprint* object, Node* list, int index);
		bool IsButtonColliding(const std::string& id, int x, int y);
		bool IsButtonColliding(Node* current, int x, int y);
		Node* GetNode(const std::string& id);

		void ReloadTree(const std::string& filename, const std::string& statename);
	};
}
