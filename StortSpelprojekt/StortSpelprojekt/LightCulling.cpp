#include "LightCulling.h"

Triangle LightCulling::TransformSpotlight(Renderer::Spotlight* spotlight)
{
	//Transforming the spotlight to a triangle
	Vec3 range = Vec3(spotlight->GetDirection()).Normalize() * spotlight->GetRange();

	Vec3 width =  range * std::sin(spotlight->GetAngle()) * 0.5f;

	Vec3 pos = Vec3(spotlight->GetPosition());
	 return Triangle(pos, pos + range + width, pos + range - width);
}

void LightCulling::GatherObjectsOnSquare(Square &square, std::vector<std::vector<GameObject*>> &allObjects)
{
	std::vector<GameObject*> gatheredObjects;

	//Loop through width/x
	for (unsigned int x = (int)square._minPos._x; x < (int)square._maxPos._x + 1; x++)
	{
		//Loop through height/y
		for (unsigned int y = (int)square._minPos._y; y < (int)square._maxPos._y + 1; y++)
		{
			gatheredObjects = _tilemap->GetAllObjectsOnTile(AI::Vec2D(x, y));
			if (!gatheredObjects.empty())
			{
				//Gather all objects from the tile and add them into the main vector
				for (unsigned int i = 0; i < gatheredObjects.size(); i++)
				{
					allObjects[gatheredObjects[i]->GetType()].push_back(gatheredObjects.at(i));
				}
			}
		}
	}
}

Ray LightCulling::CalculateFrustumEdge(float x, float y, System::Camera* camera)
{
	XMVECTOR determinant;
	XMMATRIX inverseViewMatrix = XMMatrixInverse(&determinant, *camera->GetViewMatrix());
	XMFLOAT3 mouseViewPos;

	XMStoreFloat3(&mouseViewPos, XMVector3TransformCoord(XMVectorSet(x, y, 1.0f, 0.0f), inverseViewMatrix));

	Ray ray = Ray(Vec3(camera->GetPosition()), (Vec3(mouseViewPos) - Vec3(camera->GetPosition())));

	return ray;
}


LightCulling::LightCulling()
{
	_tilemap = nullptr;
	_quadTreeRoot = nullptr;
}

LightCulling::LightCulling(Tilemap* tilemap)
{
	_tilemap = tilemap;
	_quadTreeRoot = new QuadTree(Vec2(), Vec2((float)_tilemap->GetWidth(), (float)_tilemap->GetHeight()));

	float finalQuadSize = 2 * 2;
	int nrOfDivides = (int)std::ceil(std::log2((_tilemap->GetWidth()*_tilemap->GetHeight()) / finalQuadSize) * 0.5);

	_quadTreeRoot->Divide(nrOfDivides);
}

LightCulling::~LightCulling()
{
	_tilemap = nullptr;
	_quadTreeRoot->Release();
}

std::vector<std::vector<GameObject*>> LightCulling::GetObjectsInSpotlight(Renderer::Spotlight* spotlight)
{
	Triangle triangle = TransformSpotlight(spotlight);
	std::vector<Square> collectedSquares;

	std::vector<std::vector<GameObject*>> objectsInLight;
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		objectsInLight.push_back(std::vector<GameObject*>());
	}

	_quadTreeRoot->GetSquares(triangle, collectedSquares);

	for (unsigned int i = 0; i < collectedSquares.size(); i++)
	{
		GatherObjectsOnSquare(collectedSquares[i], objectsInLight);
	}

	return objectsInLight;
}

std::vector<std::vector<GameObject*>> LightCulling::GetObjectsInFrustum(System::Camera* camera)
{
	Box cullBox;
	std::vector<Square> collectedSquares;

	std::vector<std::vector<GameObject*>> objectsInFrustum;
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		objectsInFrustum.push_back(std::vector<GameObject*>());
	}

	Ray rays[4] = { 
		CalculateFrustumEdge(-1.0f, -1.0f, camera),
		CalculateFrustumEdge(1.0f, -1.0f, camera),
		CalculateFrustumEdge(-1.0f, 1.0f, camera),
		CalculateFrustumEdge(1.0f, 1.0f, camera) };

	Vec3 points[4];
	Plane pickPlane = Plane(Vec3(), Vec3(0.0f, 1.0f, 0.0f), 0.0f);

	bool legitForReal = true;

	for (int i = 0; i < 4; i++)
	{
		if (Collision(rays[i], pickPlane))
		{
			points[i] = Intersection(rays[i], pickPlane);
		}
		else
		{
			legitForReal = false;
		}
	}

	if (legitForReal)
	{
		Vec3 pos;
		//TODO continue here
		cullBox = Box(
			pos, 
			(points[0]-points[1]).Length(),
			0.0f,
			(points[0] - points[2]).Length(), 
			Vec3(0.0f, 1.0f, 0.0f).Cross(points[0] - points[1]),
			Vec3(0.0f, 1.0f, 0.0f),
			Vec3(0.0f, 1.0f, 0.0f).Cross(points[0] - points[2])
			);
	}

	_quadTreeRoot->GetSquares(cullBox, collectedSquares);


	for (unsigned int i = 0; i < collectedSquares.size(); i++)
	{
		GatherObjectsOnSquare(collectedSquares[i], objectsInFrustum);
	}

	return objectsInFrustum;
}