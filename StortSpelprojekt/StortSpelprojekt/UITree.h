#pragma once
#include "GUI elements/Node.h"
#include "GUI elements/BlueprintNode.h"
#include "GUI elements/TextBox.h"
#include "GUI elements/ToggleButton.h"
#include "GUI elements/NodeBar.h"
#include "GUI elements/RadioButtonCollection.h"
#include "InputDevice.h"
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
		Node* LoadGUITree(const std::string& name, rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end, Node* parent = nullptr);
		Node* ParseRegularNode(rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end, Node* parent, const std::string& name);
		bool RegularNodeDataParsed(rapidjson::Value::ConstMemberIterator i, Node* returnNode);
		BlueprintNode* ParseBlueprintNode(rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end, Node* parent, const std::string& name);
		bool BlueprintNodeDataParsed(rapidjson::Value::ConstMemberIterator i, BlueprintNode* returnNode);
		void Release(Node* node);
		Node* FindNode(Node* current, const std::string& id);
	public:
		UITree(const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader);
		~UITree();

		GUI::Node* GetRootNode() const;
		void Resize(System::Settings* settings);
		void Resize(Node* current);

		int CreateBlueprintNodes(System::Blueprint* object, Node* list, int index);
		bool IsButtonColliding(const std::string& id, int x, int y);
		bool IsButtonColliding(const std::string& id, System::MouseCoord coord);
		bool IsButtonColliding(Node* current, int x, int y);
		bool IsButtonColliding(Node* current, System::MouseCoord coord);
		bool IsNodeHidden(const std::string& id);
		bool IsNodeHidden(Node* node);
		void HideNodeAndChildren(GUI::Node* node);
		void ShowNodeAndParents(GUI::Node* node);
		Node* GetNode(const std::string& id);
		void ReloadTree(const std::string& filename);
	};
}
