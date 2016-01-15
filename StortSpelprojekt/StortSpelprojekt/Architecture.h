#pragma once
#include "GameObject.h"

/*
	This should cover any game object that doesn't do anything besides existing.
	Typical examples would be walls, floors and immovable intruder objectives
	--Victor
*/
class Architecture : public GameObject
{
public:
	Architecture();
	Architecture(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject);
	~Architecture();
	void Update();
	void Release();
};
