#pragma once
#include "Shapes.h"


class QuadTree
{
private:
	Square _boundry;
	QuadTree* _children[4];
	bool isLeaf;
public:

	QuadTree(Vec2 minPoint = Vec2(), Vec2 maxPoint = Vec2());
	~QuadTree();

	void Divide(int nrOfDivisions);
	void GetSquares(Triangle& triangle, std::vector<Square> &collectedSquares);
	void GetSquares(Box frustumBox, std::vector<Square> &collectedSquares);
	void Release();

};