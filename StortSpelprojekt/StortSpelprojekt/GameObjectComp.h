#pragma once
#include "GameObject.h"
class GameObjectComp : public GameObject
{
protected:
	int _damageCompID;
	int _fovCompID;
	int _animCompID;

public:
	GameObjectComp();
	~GameObjectComp();

	int GetDamageComponentID() const;
	void BindDamageComponentID(int ID);

	int GetFOVComponentID() const;
	void BindFOVComponentID(int ID);

	int GetAnimationComponentID() const;
	void BindAnimationComponentID(int ID);

	//Update object gamelogic
	void virtual Update() = 0;

	//Release object resources
	void virtual Release() = 0;
};

