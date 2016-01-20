#pragma once

#include "Tilemap.h"
#include <DirectXMath.h>

class QuadTreeCulling
{
private:
	struct Node
	{
		DirectX::XMFLOAT2 _position;
		unsigned int _height;
		unsigned int _width;
		Node* _children;
	};


	void RecursiveCreateTree(Node* current);

public:
	QuadTreeCulling();
	~QuadTreeCulling();

	void CreateTree(Tilemap* tilemap);
};