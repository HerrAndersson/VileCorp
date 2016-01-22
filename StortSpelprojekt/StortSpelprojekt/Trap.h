#pragma once
#include "GameObject.h"
class Trap : public GameObject
{
private:
	int _cost;
public:
	Trap();
	Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject,
		int cost);
	~Trap();

	void Activate(); // TODO - Zache/Aron
	void Update();
	void Release();
};

