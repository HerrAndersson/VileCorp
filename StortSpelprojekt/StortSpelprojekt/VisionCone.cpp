#include "VisionCone.h"

void VisionCone::ScanOctant(int depth, int octant, double &startSlope, double endSlope, AI::Vec2D pos, AI::Vec2D dir)
{
	int visRangeSqrd = _radius * _radius;
	int x = 0;
	int y = 0;
	int unitPosX = pos._x;
	int unitPosY = pos._y;
	int leftToRight = 1;							//can be 1 or -1
	bool rowByRow = true;
	int endSlopeCompOffset = 1;
	int startSlopeOffset = 1;
	AI::Vec2D prevTileVec = AI::Vec2D(0, 0);
	AI::Vec2D endCornerVec = AI::Vec2D(0, 0);
	AI::Vec2D startCornerVec = AI::Vec2D(0, 0);

	switch (octant)
	{
	case 1:
		prevTileVec = AI::Vec2D(-1, 0);//In what direction the previous tile was
		endCornerVec = AI::Vec2D(-1, -1);//Is multiplied with 0.5
		startCornerVec = AI::Vec2D(-1, 1);//Is multiplied with 0.5

		y = unitPosY + depth;
		if (y < 0)
		{
			return;
		}
		x = unitPosX - (int)((startSlope * (double)depth));
		x = max(x, 0);
		break;
	case 2:
		leftToRight = -1;
		endSlopeCompOffset = -1;
		startSlopeOffset = -1;
		prevTileVec = AI::Vec2D(1, 0);
		endCornerVec = AI::Vec2D(1, -1);
		startCornerVec = AI::Vec2D(1, 1);

		y = unitPosY + depth;
		if (y >= _tileMap->GetHeight())
		{
			return;
		}
		x = unitPosX + (int)((startSlope * (double)depth));
		x = min(x, _tileMap->GetWidth() - 1);
		break;
	case 3:
		leftToRight = -1;
		rowByRow = false;
		endSlopeCompOffset = -1;
		startSlopeOffset = -1;
		prevTileVec = AI::Vec2D(0, 1);
		endCornerVec = AI::Vec2D(-1, 1);
		startCornerVec = AI::Vec2D(1, 1);

		x = unitPosX + depth;
		if (x >= _tileMap->GetWidth())
		{
			return;
		}
		y = unitPosY + (int)((startSlope * (double)depth));
		y = min(y, _tileMap->GetHeight() - 1);
		break;
	case 4:
		rowByRow = false;
		prevTileVec = AI::Vec2D(0, -1);
		endCornerVec = AI::Vec2D(-1, -1);
		startCornerVec = AI::Vec2D(1, -1);

		x = unitPosX + depth;
		if (x >= _tileMap->GetWidth())
		{
			return;
		}
		y = unitPosY - (int)((startSlope * (double)depth));
		y = max(y, 0);
		break;
	case 5:
		leftToRight = -1;
		prevTileVec = AI::Vec2D(1, 0);
		endCornerVec = AI::Vec2D(1, 1);
		startCornerVec = AI::Vec2D(1, -1);

		y = unitPosY - depth;
		if (y < 0)
		{
			return;
		}
		x = unitPosX + (int)((startSlope * (double)depth));
		x = min(x, _tileMap->GetWidth() - 1);
		break;
	case 6:
		endSlopeCompOffset = -1;
		startSlopeOffset = -1;
		prevTileVec = AI::Vec2D(-1, 0);
		endCornerVec = AI::Vec2D(-1, 1);
		startCornerVec = AI::Vec2D(-1, -1);

		y = unitPosY - depth;
		if (y < 0)
		{
			return;
		}
		x = unitPosX - (int)((startSlope * (double)depth));
		x = max(x, 0);
		break;
	case 7:
		rowByRow = false;
		endSlopeCompOffset = -1;
		startSlopeOffset = -1;
		prevTileVec = AI::Vec2D(0, -1);
		endCornerVec = AI::Vec2D(1, -1);
		startCornerVec = AI::Vec2D(-1, -1);

		x = unitPosX - depth;
		if (x < 0)
		{
			return;
		}

		y = unitPosY - (int)((startSlope * (double)depth));
		y = max(y, 0);
		break;
	case 8:
		leftToRight = -1;
		rowByRow = false;
		prevTileVec = AI::Vec2D(0, 1);
		endCornerVec = AI::Vec2D(1, 1);
		startCornerVec = AI::Vec2D(-1, 1);

		x = unitPosX - depth;
		if (x < 0)
		{
			return;
		}
		y = unitPosY + (int)((startSlope * (double)depth));
		y = min(y, _tileMap->GetHeight() - 1);
		break;
	};

	//While within octant relevant scan area
	while ((endSlopeCompOffset)* GetSlope(x, y, unitPosX, unitPosY, (!rowByRow)) >= endSlope)
	{
		if (GetVisDistance(x, y, unitPosX, unitPosY) <= visRangeSqrd)
		{
			//Is there a wall on current tile
			if (_tileMap->IsWallOnTile({x, y}))
			{
				//If previous tile was in range and not a wall
				if (_tileMap->IsValid({x, y}) && !_tileMap->IsWallOnTile({x + prevTileVec._x, y + prevTileVec._y}))
				{
					//Recurse and adjust depth with new end slope
					ScanOctant(depth + 1, octant, startSlope, (endSlopeCompOffset) * GetSlope(x + (endCornerVec._x * 0.5), y + (endCornerVec._y * 0.5), unitPosX, unitPosY, (!rowByRow)), pos, dir);
				}
			}
			else
			{
				if ((_tileMap->IsWallOnTile({x + (prevTileVec._x), y + (prevTileVec._y)})))
				{
					startSlope = (startSlopeOffset)* GetSlope(x + (startCornerVec._x * 0.5), y + (startCornerVec._y * 0.5), unitPosX, unitPosY, (!rowByRow));
				}
				//Add current tile to visible
				_visibleTiles[_nrOfVisibleTiles] = {x, y};
				_nrOfVisibleTiles++;
			}
		}
		if (rowByRow)			//If row by row, use x
		{
			x += leftToRight;
		}
		else					//Else use y 
		{
			y += leftToRight;
		}
	}
	if (rowByRow)				//If row by row, use x
	{
		x -= leftToRight;
	}
	else						//Else use y
	{
		y -= leftToRight;
	}

	x = min(max(x, 0), _tileMap->GetWidth() - 1);	//Set x within tile range if it's outside
	y = min(max(y, 0), _tileMap->GetHeight() - 1);	//Set y within tile range if it's outside

													//Recurse and run if depth is within range
	if (depth < _radius && (!_tileMap->IsWallOnTile({x, y})))
	{
		ScanOctant(depth + 1, octant, startSlope, endSlope, pos, dir);
	}
}

double VisionCone::GetSlope(double x1, double y1, double x2, double y2, bool invert)
{
	if (invert)
	{
		return -((y1 - y2) / (x1 - x2));
	}
	else
	{
		return -((x1 - x2) / (y1 - y2));
	}
}

int VisionCone::GetVisDistance(int x1, int y1, int x2, int y2)
{
	return (x1 - x2) * (x1 - x2) + ((y1 - y2) * (y1 - y2));
}

VisionCone::VisionCone()
{
	_radius = 0;
	_tileMap = nullptr;
	_visibleTiles = nullptr;
	_nrOfVisibleTiles = 0;
}

VisionCone::VisionCone(int radius, const Tilemap* tileMap)
{
	_radius = radius;
	_tileMap = tileMap;
	_visibleTiles = new AI::Vec2D[radius * (radius + 1)];
	_nrOfVisibleTiles = 0;
}

VisionCone::~VisionCone()
{
	delete[] _visibleTiles;
	_visibleTiles = nullptr;
	_nrOfVisibleTiles = 0;
}

void VisionCone::FindVisibleTiles(AI::Vec2D pos, AI::Vec2D dir)
{
	double startSlope = 1.0;
	_visibleTiles[0] = AI::Vec2D(pos._x, pos._y);
	_nrOfVisibleTiles = 1;
	int octant = 1;
	int c = 0;

	if (dir._x == -1)
	{
		octant = 7 + dir._y;
	}
	else
	{
		octant = 3 - (2 - dir._x) * dir._y;			//2-4 if x = 1, 1 or 5 if x = 0
	}
	ScanOctant(1, octant, startSlope, 0.0, pos, dir);
	octant = octant % 8 + 1;
	startSlope = 1.0;
	ScanOctant(1, octant, startSlope, 0.0, pos, dir);
}

AI::Vec2D * VisionCone::GetVisibleTiles() const
{
	return _visibleTiles;
}

AI::Vec2D VisionCone::GetVisibleTile(int index) const
{
	return _visibleTiles[index];
}

int VisionCone::GetNrOfVisibleTiles() const
{
	return _nrOfVisibleTiles;
}

void VisionCone::ColorVisibleTiles(DirectX::XMFLOAT3 color)
{
	for (int i = 0; i < _nrOfVisibleTiles; i++)
	{
		GameObject* floor = _tileMap->GetObjectOnTile(_visibleTiles[i], System::FLOOR);
		if (floor != nullptr)
		{
			floor->SetColorOffset(color);
		}
	}
}
