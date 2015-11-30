#include "Tile.h"
#include <iostream>



Tile::Tile()
{
	for (int i = 0; i < OBJECT_CAPACITY; i++)
	{
		_objectsOnTile[i] = nullptr;
	}
}

Tile::~Tile() 
{}



void Tile::Update()
{
	// TODO: Check std::vector - Zache/Aron
}

void Tile::Release()
{
	// TODO: The soo what question - Kryztof Wnuk
	//_objectsOnTile.clear();
}

GameObject* Tile::GetGameObject(int index) const
{
	if (index < 0 || index > 3)
	{
		return nullptr;
	}
	else
	{
		return _objectsOnTile[index];
	}
}

bool Tile::IsWall() const
{
	return _objectsOnTile[0]->GetType()== WALL;
}

int Tile::UnitsOnTile() const
{
	return _objectsOnTile[0]->GetType() == UNIT;
}

bool Tile::IsTrapOnTile() const
{
	return _objectsOnTile[0]->GetType() == TRAP;
}

bool Tile::IsTriggerOnTile() const
{
	return _objectsOnTile[0]->GetType() == TRIGGER;
}

bool Tile::IsObjectiveOnTile() const
{
	return _objectsOnTile[0]->GetType() == LOOT;
}
