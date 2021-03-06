#pragma once
#include "GameObject.h"

/*
	This should cover any game object that doesn't do anything besides existing.
	Typical examples would be walls, floors and immovable intruder objectives
	--Victor
*/
class Architecture : public GameObject
{
private:
	bool _isNoPlacementZone;
public:
	Architecture(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, Renderer::ParticleEventQueue* particleEventQueue, int subType = 0, AI::Vec2D direction = { 1, 0 });
	~Architecture();
	void Update(float deltaTime);
	void Release();
	bool InRange(AI::Vec2D pos)const;
	bool GetNoPlacementZone() const;
	void SetNoPlacementZone(bool val);
};
