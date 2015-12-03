#pragma once
#include "GameObject.h"
class Architecture : public GameObject
{
public:
	Architecture();
	Architecture(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject);
	~Architecture();
	void Update();
	void Release();
};
