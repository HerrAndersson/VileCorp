#pragma once
#include "Node.h"
#include "AssetManager.h"
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"

#include <fstream>
#include <locale>
#include <codecvt>
#include <string>

namespace GUI
{
	class UITree
	{
	private:
		Node* _root;
		AssetManager* _AM;
		FontWrapper* _fontWrapper;
	private:
		Node* LoadGUITree(const std::string& name, rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end);
		void Release(Node* node);
	public:
		UITree(const std::string& filename, const std::string& statename, AssetManager* assetManager, FontWrapper* fontWrapper);
		~UITree();

		GUI::Node* GetRootNode() const;
	};
}
