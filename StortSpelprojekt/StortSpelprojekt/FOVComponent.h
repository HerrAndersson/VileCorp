#pragma once
#include "Component.h"

/*
TODO: Zache/Aron
Variabler: Kon(vinkel, längd)
*/

class FOVComponent : public Component
{
private:
	int _angle;
	float _length;
public:
	FOVComponent();
	~FOVComponent();
};

