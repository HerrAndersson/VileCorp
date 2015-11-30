#pragma once

/*
GameObject class
Base interface for specialized GameObjects.

Contains an unique ID for identification, a world position and a enum Type (TODO come up with a good system for types, (string/char*, or maybe new hierchical classes?)
If the object doesn't need a _renderObject, set it to nullptr.
If the object has a renderObject but is out of sight _visibility will be false.
*/

// TODO: Behöver spara världsmatriser

//placeholder
struct Vec3
{
	float _posX, _posY, _posZ;
};

//Preliminary solution  - Zache/Marcus
enum Type {UNIT, STRUCTURE, TILE, TRAP, TRIGGER };

class GameObject
{
protected:
	unsigned short _ID;
	Vec3 _position;
	//Vec3 _direction;
	Type _type;
	bool _visible; // OBS
	int _renderObjectID;

public:
	GameObject();
	~GameObject();

	short GetID() const;

	//TODO change Vec3 to XMVECTOR or other vectorclass - Zache/Marcus
	Vec3 GetPosition();
	void SetPosition(Vec3 position);

	Type GetType() const;

	bool IsVisible() const;
	void SetVisability(bool visible);

	void SetRenderObjectID(int renderObjectID);
	int GetRenderObjectID() const;

	//Update object gamelogic
	void virtual Update() = 0;

	//Release object resources
	void virtual Release() = 0;
};

