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
	std::vector<std::vector<GameObject*>>* _objectsInLight;

	//LightCulling
	std::vector<Vec2>* TransformSpotlight(Renderer::Spotlight* spotlight);

	//FrustumCulling
	//Ray CalculateFrustumEdge(float x, float y, System::Camera* camera);

public:

	LightCulling();
	LightCulling(Tilemap* tilemap);
	~LightCulling();

	std::vector<std::vector<GameObject*>>* GetObjectsInSpotlight(Renderer::Spotlight* spotlight);
	//std::vector<std::vector<GameObject*>> GetObjectsInFrustum(System::Camera* camera);

};

