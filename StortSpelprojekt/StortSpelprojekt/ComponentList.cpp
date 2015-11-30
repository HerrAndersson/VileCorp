#include "ComponentList.h"



ComponentList::ComponentList()
{
	_size = 0;
}


ComponentList::~ComponentList()
{
	for (int i = 0; i < _size; i++)
	{
		delete components[i];
	}
	components.clear();
}

int ComponentList::AddComponent(Component* comp)
{
	int index = -1;
	bool exists = false;

	for (int i = 0; i < _size && !exists; i++)
	{
		if (components[i] == comp)
		{
			index = i;
			exists = true;
		}
	}

	if (!exists)
	{
		components.push_back(comp);
		index = _size;
		_size++;
	}

	return index;
}

Component* ComponentList::GetComponent(int index) const
{
	return components[index];
}

int ComponentList::GetSize() const
{
	return _size;
}
