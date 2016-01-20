#include "QuadTreeCulling.h"

QuadTreeCulling::QuadTreeCulling()
{}

QuadTreeCulling::~QuadTreeCulling()
{}

void QuadTreeCulling::CreateTree(Tilemap* tileMap)
{
	Node root;
	root._position = DirectX::XMFLOAT2(0, 0);
	root._height = tileMap->GetHeight();
	root._width = tileMap->GetWidth();
	
	RecursiveCreateTree(&root);
}

void QuadTreeCulling::RecursiveCreateTree(Node* current)
{
	
}