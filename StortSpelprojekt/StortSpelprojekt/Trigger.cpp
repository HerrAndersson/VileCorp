#include "Trigger.h"



Trigger::Trigger()
{
	_size = 0;
	_traps = std::vector<Trap*>();
}

Trigger::Trigger(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{}


Trigger::~Trigger()
{}

bool Trigger::ConnectTrap(Trap * trap)
{
	bool exist = false;

	for (int i = 0; i < _size; i++)
	{
		if (_traps[i] == trap)
		{
			exist = true;
		}
	}

	if (!exist)
	{
		_traps.push_back(trap);
	}

	return exist;
}

bool Trigger::DisconnectTrap(short ID)
{
	bool removed = false;

	for (int i = 0; i < _size; i++)
	{
		if (_traps[i]->GetID() == ID)
		{
			// Decrease size of vector
			_size--;

			// Replace pointer with the last pointer int the vector
			_traps[i] = _traps[_size];

			// Remove pointer value to avoid various problems
			_traps.pop_back();

			removed = true;
		}
	}

	return removed;
}

void Trigger::ActivateTraps()
{
	for (int i = 0; i < _size; i++)
	{
		_traps[i]->Activate();
	}
}

void Trigger::Update(float deltaTime)
{
}

void Trigger::Release()
{
}
