#include "Trap.h"
#include <sstream>

Trap::Trap() : GameObjectComp()
{
	_functioning = true;
}

Trap::Trap(const Trap & copy)
{
	// Kanske flytta till GameObject
	_type = copy._type;
	_renderObjectID = copy._renderObjectID;

	// Kanske flytta till GameObjectComp
	_damageCompID = copy._damageCompID;
	_fovCompID = copy._fovCompID;
	_animCompID = copy._animCompID;

	_functioning = copy._functioning;
}


Trap::~Trap()
{
}

void Trap::Fire()
{
}

void Trap::Enable()
{
	_functioning = true;
}

void Trap::Disable()
{
	_functioning = false;
}

std::string Trap::GetInfo()
{
	std::stringstream ss; 
	
	ss << "Damage Component ID: " << _damageCompID << " and functioning: " << _functioning << std::endl;

	return ss.str();
}
