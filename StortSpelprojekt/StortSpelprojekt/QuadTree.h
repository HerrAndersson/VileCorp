#pragma once
#include "Shapes.h"
#include "AIUtil.h"
#include "GameObject.h"
#include "Tilemap.h"

class QuadTree
{
private:
	std::vector<Vec2> _boundry; //Four points of an aligned square
	QuadTree* _children[4];
	bool _isLeaf;
	std::vector<AI::Vec2D> _tiles; //Will be empty for all nodes except leaves
public:
	QuadTree(Vec2 minPoint = Vec2(), Vec2 maxPoint = Vec2());
	~QuadTree();

	void Divide(int nrOfDivisions);
	void GetObjects(std::vector<Vec2>* polygon, std::vector<std::vector<GameObject*>>* collectedObjects, Tilemap* tilemap);
	void Release();

};