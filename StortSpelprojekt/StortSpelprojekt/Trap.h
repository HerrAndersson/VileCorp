#pragma once
#include "GameObject.h"
class Trap : public GameObject
{
public:
	Trap();
	Trap(unsigned short ID, Vec3 position, RenderObject * renderObject);
	~Trap();

	void Activate(); // TODO - Zache/Aron
};

