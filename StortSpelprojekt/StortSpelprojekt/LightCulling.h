#pragma once
#include "QuadTree.h"
#include "Tilemap.h"
#include "GameObject.h"
#include "Spotlight.h"
#include "Camera.h"
/*
Handles both culling for lightning purposes and camera frustum purposes

*/
class LightCulling
{
private:
	QuadTree* _quadTreeRoot;
	Tilemap* _tilemap;

	//LightCulling
	Triangle TransformSpotlight(Renderer::Spotlight* spotlight);
	void GatherObjectsOnSquare(Square &square, std::vector<std::vector<GameObject*>> &allObjects);

	//FrustumCulling
	Ray CalculateFrustumEdge(float x, float y, System::Camera* camera);

public:
	LightCulling();
	LightCulling(Tilemap* tilemap);
	~LightCulling();

	std::vector<std::vector<GameObject*>> GetObjectsInSpotlight(Renderer::Spotlight* spotlight);
	std::vector<std::vector<GameObject*>> GetObjectsInFrustum(System::Camera* camera);

};

