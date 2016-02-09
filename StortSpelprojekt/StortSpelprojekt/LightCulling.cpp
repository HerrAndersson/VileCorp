#include "LightCulling.h"

std::vector<Vec2>* LightCulling::TransformSpotlight(Renderer::Spotlight* spotlight)
{
	//Transforming the spotlight to a triangle
	Vec3 direction = Vec3(spotlight->GetDirection()).Normalize();
	Vec3 range = direction * spotlight->GetRange() * 1.2f;
	Vec3 width = direction.Cross(Vec3(0.0f, 1.0f, 0.0f)) * (range * std::sin(spotlight->GetAngle()*0.5f)).Length();
	Vec3 pos = Vec3(spotlight->GetPosition());

	std::vector<Vec2>* triangle = new std::vector<Vec2>();
	triangle->push_back(Vec2(pos._x, pos._z));
	triangle->push_back(Vec2(pos._x, pos._z) + Vec2(range._x, range._z) + Vec2(width._x, width._z));
	triangle->push_back(Vec2(pos._x, pos._z) + Vec2(range._x, range._z) - Vec2(width._x, width._z));

	return triangle;
}

//Ray LightCulling::CalculateFrustumEdge(float x, float y, System::Camera* camera)
//{
//	XMVECTOR determinant;
//	XMMATRIX inverseViewMatrix = XMMatrixInverse(&determinant, *camera->GetViewMatrix());
//	XMFLOAT3 mouseViewPos;
//
//	XMStoreFloat3(&mouseViewPos, XMVector3TransformCoord(XMVectorSet(x, y, 1.0f, 0.0f), inverseViewMatrix));
//
//	Ray ray = Ray(Vec3(camera->GetPosition()), (Vec3(mouseViewPos) - Vec3(camera->GetPosition())));
//
//	return ray;
//}
//

LightCulling::LightCulling()
{
	_tilemap = nullptr;
	_quadTreeRoot = nullptr;
	_objectsInLight = nullptr;
}

LightCulling::LightCulling(Tilemap* tilemap)
{
	_tilemap = tilemap;
	_quadTreeRoot = new QuadTree(Vec2(), Vec2((float)_tilemap->GetWidth(), (float)_tilemap->GetHeight()));

	_objectsInLight = new std::vector<std::vector<GameObject*>>();
	_objectsInLight->reserve(NR_OF_TYPES);

	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		_objectsInLight->push_back(std::vector<GameObject*>());
	}
	//The quadtree will stop dividing when the quads are x * x small
	float finalQuadSize = 4 * 4;
	//Mathemagic!
	int nrOfDivides = (int)std::ceil(std::log2((_tilemap->GetWidth()*_tilemap->GetHeight()) / finalQuadSize) * 0.5);

	_quadTreeRoot->Divide(nrOfDivides);
}

LightCulling::~LightCulling()
{
	_tilemap = nullptr;

	SAFE_DELETE(_objectsInLight);
	SAFE_DELETE(_quadTreeRoot);
}

std::vector<std::vector<GameObject*>>* LightCulling::GetObjectsInSpotlight(Renderer::Spotlight* spotlight)
{
	std::vector<Vec2>* triangle = TransformSpotlight(spotlight);

	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		_objectsInLight->at(i).clear();
	}
	_quadTreeRoot->GetObjects(triangle, _objectsInLight, _tilemap);

	delete triangle;
	return _objectsInLight;
}

//std::vector<std::vector<GameObject*>> LightCulling::GetObjectsInFrustum(System::Camera* camera)
//{
//	std::vector<Vec2> frustum;
//	std::vector<AI::Vec2D> collectedTiles;
//
//	std::vector<std::vector<GameObject*>> objectsInFrustum;
//
//	for (int i = 0; i < NR_OF_TYPES; i++)
//	{
//		objectsInFrustum.push_back(std::vector<GameObject*>());
//	}
//
//	/////////////////
//	Ray rays[2] = {
//		CalculateFrustumEdge(-2.0f, 0.0f, camera),
//		CalculateFrustumEdge(2.0f, 0.0f, camera) };
//
//
//	Vec2 temp = Vec2(rays[0]._origin._x, rays[0]._origin._z);
//	frustum.push_back(temp);
//	frustum.push_back(temp + (Vec2(rays[0]._direction._x, rays[0]._direction._z)) * 50);
//	frustum.push_back(temp + (Vec2(rays[1]._direction._x, rays[1]._direction._z)) * 50);
//
//
//	////////////////////////
//	//Ray rays[4] = {
//	//	CalculateFrustumEdge(-1.0f, -1.0f, camera),
//	//	CalculateFrustumEdge(1.0f, -1.0f, camera),
//	//	CalculateFrustumEdge(-1.0f, 1.0f, camera),
//	//	CalculateFrustumEdge(1.0f, 1.0f, camera) };
//
//
//	//Plane pickPlane = Plane(Vec3(), Vec3(0.0f, 1.0f, 0.0f), 0.0f);
//
//	//for (int i = 0; i < 4; i++)
//	//{
//	//	if (Collision(rays[i], pickPlane))
//	//	{
//	//		Vec3 point = Intersection(rays[i], pickPlane);
//
//	//		frustum.push_back(Vec2(point._x, point._z));
//	//	}
//	//}
//	///////////////////////////
//
//	_quadTreeRoot->GetTiles(frustum, collectedTiles);
//
//	GatherObjectsOnTiles(collectedTiles, objectsInFrustum);
//	
//
//	return objectsInFrustum;
//}