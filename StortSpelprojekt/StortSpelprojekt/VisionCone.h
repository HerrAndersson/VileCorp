#pragma once
#include "Tilemap.h"
#include "AIUtil.h"

class VisionCone
{
private:
	const Tilemap* _tileMap;
	int _radius;
	AI::Vec2D* _visibleTiles;
	int _nrOfVisibleTiles;

	void ScanOctant(int depth, int octant, double &startSlope, double endSlope, AI::Vec2D pos, AI::Vec2D dir);
	double GetSlope(double x1, double y1, double x2, double y2, bool invert);
	int GetVisDistance(int x1, int y1, int x2, int y2);
public:
	VisionCone();
	VisionCone(int radius, const Tilemap* tileMap);
	~VisionCone();
	void FindVisibleTiles(AI::Vec2D pos, AI::Vec2D dir);

	AI::Vec2D* GetVisibleTiles()const;
	AI::Vec2D GetVisibleTile(int index)const;
	int GetNrOfVisibleTiles()const;
};