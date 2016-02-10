#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include "../Tilemap.h"
#include "AStar.h"
#include "../VisionCone.h"
class SecurityCamera : public GameObject
{
private:
	const Tilemap* _tileMap;
	AI::Vec2D _direction;
	int _visionRadius;
	VisionCone* _visionCone;
	void Rotate();

public:
	SecurityCamera();
	SecurityCamera(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap);
	virtual ~SecurityCamera();
	AI::Vec2D GetDirection() const;
	void SetDirection(AI::Vec2D direction);
	void CheckVisibleTiles();
	void Update(float deltaTime);
	void Release();

};

