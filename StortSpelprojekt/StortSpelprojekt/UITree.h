#pragma once
#include "Node.h"
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"

#include <fstream>

namespace GUI
{
	class UITree
	{
	private:
		Node* _root;
	private:
		void LoadGUITree(Node* current, rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end);
		void Release(Node* node);
	public:
		UITree(const std::string & filename, const std::string & statename);
		~UITree();

		GUI::Node* GetRootNode() const;
	};
}
