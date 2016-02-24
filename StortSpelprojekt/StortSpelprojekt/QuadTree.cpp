#include "QuadTree.h"

QuadTree::QuadTree(Vec2 minPoint, Vec2 maxPoint)
{
	//3 _______ 2
	// |       |
	// |       |
	// |       |
	//0|_______|1

	_boundry.push_back(minPoint);
	_boundry.push_back(Vec2(maxPoint._x, minPoint._y));
	_boundry.push_back(maxPoint);
	_boundry.push_back(Vec2(minPoint._x, maxPoint._y));

	for (int i = 0; i < 4; i++)
	{
		_children[i] = nullptr;
	}
	_isLeaf = true;
	_tiles = std::vector<AI::Vec2D>();
}

QuadTree::~QuadTree()
{
	for (int i = 0; i < 4; i++)
	{
		SAFE_DELETE(_children[i]);
	}
}

void QuadTree::Divide(int nrOfDivisions)
{
	if (nrOfDivisions > 0)
	{
		int nrOfDivisionsLeft = --nrOfDivisions;
		_isLeaf = false;
		//Middlepoint between min point and max point
		Vec2 midPos = (_boundry.at(0) + _boundry.at(2))*0.5f;
		//First child
		//	
		//	|_____
		//	|     |
		//	|_____|__
		_children[0] = new QuadTree(_boundry.at(0), midPos);
		_children[0]->Divide(nrOfDivisions);

		//Second child
		//	   
		//	   _____|
		//	  |     |
		//	__|_____|
		_children[1] = new QuadTree(Vec2(midPos._x, _boundry.at(0)._y), Vec2(_boundry.at(2)._x, midPos._y));
		_children[1]->Divide(nrOfDivisions);

		//Third child
		//	 _______
		//	  |     |
		//	  |_____|
		//	        |
		_children[2] = new QuadTree(midPos, _boundry.at(2));
		_children[2]->Divide(nrOfDivisions);

		//Fourth child
		//	 _______
		//	|     |
		//	|_____|
		//	|
		_children[3] = new QuadTree(Vec2(_boundry.at(0)._x, midPos._y), Vec2(midPos._x, _boundry.at(2)._y));
		_children[3]->Divide(nrOfDivisions);
	}
	else
	{
		//Loop through width/x
		for (int x = (int)_boundry.at(0)._x; x < (int)_boundry.at(2)._x+1; x++)
		{
			//Loop through height/y
			for (int y = (int)_boundry.at(0)._y; y < (int)_boundry.at(2)._y+1; y++)
			{
				_tiles.push_back(AI::Vec2D(x, y));
			}
		}
	}
}
void QuadTree::GetObjects(std::vector<Vec2>* polygon, std::vector<std::vector<GameObject*>>* collectedObjects, Tilemap* tilemap)
{
	if (Collision(polygon, &_boundry))
	{
		if (_isLeaf)
		{
			//Gather all objects from the tile and add them into the main vector
			unsigned int size = _tiles.size();
			for (unsigned int i = 0; i < size; i++)
			{
				int x = _tiles.at(i)._x;
				int y = _tiles.at(i)._y;

				//Start on 1 to skip the floor
				for (unsigned int j = 1; j < NR_OF_TYPES; j++)
				{
					GameObject* object = tilemap->GetObjectOnTile(x, y, Type(j));
					if (object != nullptr)
					{
						collectedObjects->at(j).push_back(object);
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				_children[i]->GetObjects(polygon, collectedObjects, tilemap);
			}
		}
	}
}

