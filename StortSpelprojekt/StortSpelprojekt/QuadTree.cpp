#include "QuadTree.h"

QuadTree::QuadTree(Vec2 minPoint, Vec2 maxPoint)
{
	_boundry = Square(minPoint, maxPoint);
	for (int i = 0; i < 4; i++)
	{
		_children[i] = nullptr;
	}
	isLeaf = true;
}
QuadTree::~QuadTree()
{

}

void QuadTree::Divide(int nrOfDivisions)
{
	
	if (nrOfDivisions > 0) 
	{
		int nrOfDivisionsLeft = --nrOfDivisions;
		isLeaf = false;
		Vec2 midPos = (_boundry._minPos + _boundry._maxPos)*0.5f;
		//First child
		//	
		//	|_____
		//	|     |
		//	|_____|__
		_children[0] = new QuadTree(_boundry._minPos, midPos);
		_children[0]->Divide(nrOfDivisions);

		//Second child
		//	   
		//	   _____|
		//	  |     |
		//	__|_____|
		_children[1] = new QuadTree(Vec2(midPos._x, _boundry._minPos._y), Vec2(_boundry._maxPos._x, midPos._y));
		_children[1]->Divide(nrOfDivisions);

		//Third child
		//	 _______
		//	|     |
		//	|_____|
		//	|
		_children[2] = new QuadTree(Vec2(_boundry._minPos._x, midPos._y), Vec2( midPos._x, _boundry._maxPos._y ));
		_children[2]->Divide(nrOfDivisions);

		//Fourth child
		//	 _______
		//	  |     |
		//	  |_____|
		//	        |
		_children[3] = new QuadTree(midPos, _boundry._maxPos);
		_children[3]->Divide(nrOfDivisions);
	}

}
void QuadTree::GetSquares(Triangle& triangle, std::vector<Square>&collectedSquares)
{
	if (Collision(triangle, _boundry))
	{
		if (isLeaf)
		{
			collectedSquares.push_back(_boundry);
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				_children[i]->GetSquares(triangle, collectedSquares);
			}
		}
	}
}

void QuadTree::GetSquares(Box frustumBox, std::vector<Square>&collectedSquares)
{
	if (Collision(frustumBox, _boundry))
	{
		if (isLeaf)
		{
			collectedSquares.push_back(_boundry);
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				_children[i]->GetSquares(frustumBox, collectedSquares);
			}
		}
	}
}

void QuadTree::Release()
{
	if (_children[0] != nullptr)
	{
		for (int i = 0; i < 4; i++)
		{
			_children[i]->Release();
			delete _children[i];
		}
	}
}
