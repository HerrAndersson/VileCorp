#pragma once

#include "Tilemap.h"
#include <DirectXMath.h>
#include "Shapes.h"

/*
Children order:
 ___ ___
| 0 | 1 |
|___|___|
| 3 | 2 |
|___|___|

This implementation is most efficient when tilemap is square or close to square.
Memory clear right now is not implemented for sparse quad trees!
*/

class QuadTreeCulling
{
private:
	struct Node
	{
		DirectX::XMFLOAT2 _position;	//Lower left corner of "box"
		float _height;
		float _width;
		Node* _children[4];

		Node(float height = 0.0f, float width = 0.0f)
		{
			_height = height;
			_width = width;
		}
	};

	Node _root;

	//Helpers
	void ReleaseNode(Node* node);
	void RecursiveCreateTree(Node* current, DirectX::XMFLOAT2 position);

public:
	QuadTreeCulling();
	~QuadTreeCulling();

	void CreateTree(Tilemap* tilemap);

	//Can be overloaded with different kind of indata
	void GetRelevantTiles(Triangle triangle);
};