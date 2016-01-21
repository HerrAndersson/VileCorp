#include "QuadTreeCulling.h"

QuadTreeCulling::QuadTreeCulling()
{
	_root = Node();
}

QuadTreeCulling::~QuadTreeCulling()
{
	ReleaseNode(&_root);
}

/*
Helpers
*/
void QuadTreeCulling::ReleaseNode(Node* node)
{
	//If [0] == nullptr, they all are
	if (node->_children[0] != nullptr)
	{
		ReleaseNode(node->_children[0]);
		delete node->_children[0];
		node->_children[0] = nullptr;
		ReleaseNode(node->_children[1]);
		delete node->_children[1];
		node->_children[1] = nullptr;
		ReleaseNode(node->_children[2]);
		delete node->_children[2];
		node->_children[2] = nullptr;
		ReleaseNode(node->_children[3]);
		delete node->_children[3];
		node->_children[3] = nullptr;
	}
}

void QuadTreeCulling::RecursiveCreateTree(Node* current, DirectX::XMFLOAT2 position)
{
	current->_position = position;

	current->_children[0] = nullptr;
	current->_children[1] = nullptr;
	current->_children[2] = nullptr;
	current->_children[3] = nullptr;

	if (current->_width > 4.0f || current->_height > 4.0f)
	{
		//Child 0
		current->_children[0] = new Node(current->_height / 2, current->_width / 2);
		DirectX::XMFLOAT2 newPos = DirectX::XMFLOAT2(current->_position.x,
			current->_position.y + (current->_height / 2));
		RecursiveCreateTree(current->_children[0], newPos);
		//Child 1
		current->_children[1] = new Node(current->_height / 2, current->_width / 2);
		newPos = DirectX::XMFLOAT2(current->_position.x + (current->_width / 2),
			current->_position.y + (current->_height / 2));
		RecursiveCreateTree(current->_children[1], newPos);
		//Child 2
		current->_children[2] = new Node(current->_height / 2, current->_width / 2);
		newPos = DirectX::XMFLOAT2(current->_position.x + (current->_width / 2),
			current->_position.y);
		RecursiveCreateTree(current->_children[2], newPos);
		//Child 3
		current->_children[3] = new Node(current->_height / 2, current->_width / 2);
		newPos = DirectX::XMFLOAT2(current->_position.x,
			current->_position.y);
		RecursiveCreateTree(current->_children[3], newPos);
	}



}

/*
Public functions
*/

void QuadTreeCulling::CreateTree(Tilemap* tileMap)
{
	_root = Node(tileMap->GetHeight(), tileMap->GetWidth());
	
	RecursiveCreateTree(&_root, DirectX::XMFLOAT2(0, 0));
}

void QuadTreeCulling::GetRelevantTiles(Triangle triangle)
{
	
}