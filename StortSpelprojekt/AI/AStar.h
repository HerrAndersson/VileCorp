#pragma once
#include <cmath>
#include <algorithm>
#include <fstream>
#include "Heap.h"
#include "AIUtil.h"

#define AI_EXPORT __declspec(dllexport)

namespace AI
{
	/*
		Basic A* algorithm operating on a map with varying tile cost.
		Should be relatively easy to expand for more specialized behavior
		--Victor
	*/
	class AI_EXPORT AStar
	{
	public:
		

		/*
		Different heuristic used for estimating the distance to the goal
		MANHATTAN: No diagonal movement
		CHEBYSHEV: Diagonal movement has a cost of 1
		OCTILE: Diagonal movement costs sqrt(2)
		EUCLIDEAN: Calculates the distance in a straight line to the goal.
		*/
		enum Heuristic
		{
			MANHATTAN, CHEBYSHEV, OCTILE, EUCLIDEAN
		};
	private:
		struct Node
		{
			Vec2D _position;
			__int8 _open;										//0 = not checked, 1 = open, 2 = closed
			__int16 _tileCost;									//cost of traversing the individual tile
			float _gCost, _hCost;								//distance from start and heuristic to goal, respectively
			Node* _parent;										//the path back to the start node 
			Node()
			{
				_position = {0, 0};
				_hCost = -1.0f;
				_gCost = -1.0f;
				_open = 0;
				_parent = nullptr;
				_tileCost = 0;
			}
			Node(__int16 x, __int16 y, float h = -1.0f)
			{
				_position = {x, y};
				_hCost = h;
				_gCost = -1.0f;
				_open = 0;
				_parent = nullptr;
				_tileCost = 0;
			}
			~Node()
			{}
			bool operator<(const Node& comp)
			{
				return (_gCost + _hCost) < (comp._gCost + comp._hCost);
			}
			bool operator>(const Node& comp)
			{
				return (_gCost + _hCost) > (comp._gCost + comp._hCost);
			}
		};
		int _pathLength;
		Vec2D* _path;											//An ordered array moving from goal to start
		__int16 _height, _width;
		Node** _grid;
		//	std::vector<Node> _openQueue;							//A priority queue for open nodes
		Heap<Node> _openQueue;
		Vec2D _start, _goal;
		Heuristic _heuristicType;
		__int8 _hWeight;										//heuristic weight for moving a tile
		bool IsPositionValid(Vec2D pos);
		void CalculateHCost(Vec2D pos);
		void CalculateGCost(Vec2D parentPos, Vec2D currentPos);
	public:
		AStar();
		AStar(int width, int height, Vec2D start, Vec2D goal, Heuristic heuristic = MANHATTAN, int hWeight = 1);
		AStar(int width, int height, Heuristic heuristic = MANHATTAN, int hWeight = 1);
		virtual ~AStar();
		void SetTileCost(Vec2D pos, int cost = 1);
		void SetStartPosition(Vec2D pos);
		void SetGoalPosition(Vec2D pos);
		int GetTileCost(Vec2D pos)const;
		Vec2D* GetPath() const;
		int GetPathLength() const;
		float GetHeuristicDistance(Vec2D start, Vec2D goal) const;
		void CleanMap();
		void Init(Vec2D start, Vec2D goal);
		bool FindPath();

		void printMap();
	};

}