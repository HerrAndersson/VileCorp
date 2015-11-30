#pragma once
#include <vector>
#include "Component.h"
#include "DamageComponent.h"
#include "fovComponent.h"

class ComponentList
{
private:
	int _size;
	std::vector<Component*> components;
public:
	ComponentList();
	~ComponentList();
	
	// Adds component if it does not exist. Returns the index (even if it already exists).
	int AddComponent(Component* comp);
	Component* GetComponent(int index) const;

	int GetSize() const;
};

