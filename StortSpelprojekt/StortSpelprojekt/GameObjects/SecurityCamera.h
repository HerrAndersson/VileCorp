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
	int _visionRadius;
	VisionCone* _visionCone;
	void Rotate();

public:
	SecurityCamera(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, Renderer::ParticleEventQueue* particleEventQueue, const Tilemap* tileMap, AI::Vec2D direction = { 1, 0 });
	virtual ~SecurityCamera();
	AI::Vec2D GetDirection() const;
	void SetDirection(const AI::Vec2D direction);
	void CheckVisibleTiles();
	void Update(float deltaTime);
	void Release();
	void SetTilePosition(AI::Vec2D pos);
	int GetVisionRadius()const;

	//// Overloaded because of visibility
	void ShowAreaOfEffect();

};

