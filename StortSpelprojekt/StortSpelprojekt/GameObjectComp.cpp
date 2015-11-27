#include "GameObjectComp.h"



GameObjectComp::GameObjectComp()
{
	_damageCompID = -1;
	_fovCompID = -1;
	_animCompID = -1;
}


GameObjectComp::~GameObjectComp()
{}

int GameObjectComp::GetDamageComponentID() const
{
	return _damageCompID;
}

void GameObjectComp::BindDamageComponentID(int ID)
{
	_damageCompID = ID;
}

int GameObjectComp::GetFOVComponentID() const
{
	return _fovCompID;
}

void GameObjectComp::BindFOVComponentID(int ID)
{
	_fovCompID = ID;
}

int GameObjectComp::GetAnimationComponentID() const
{
	return _animCompID;
}

void GameObjectComp::BindAnimationComponentID(int ID)
{
	_animCompID = ID;
}
