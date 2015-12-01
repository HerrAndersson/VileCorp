#pragma once
#include <DirectXMath.h>

/*
GameObject class
Base interface for specialized GameObjects.

Contains an unique ID for identification, a world position and a enum Type (TODO come up with a good system for types, (string/char*, or maybe new hierchical classes?)
If the object doesn't need a _renderObject, set it to nullptr.
If the object has a renderObject but is out of sight _visibility will be false.
*/

	Vec3& operator=(const Vec3& comp)
	{
		_posX = comp._posX;
		_posY = comp._posY;
		_posZ = comp._posZ;
		return *this;
	}
//Preliminary solution  - Zache/Marcus
enum Type {UNIT, WALL, FLOOR, TRAP, TRIGGER, LOOT };

using namespace DirectX;

class GameObject
{
protected:
	unsigned short _ID;
	XMFLOAT3 _position;
	XMFLOAT3 _direction;
	XMFLOAT3 _scale;

	Type _type;
	bool _visible; // OBS
	int _renderObjectID;

public:
	GameObject();
	//Type might not be necessary, depending on whether subclasses can correspond to one type or many.
	GameObject(unsigned short ID, Vec3 position, Type type, RenderObject* renderObject);
	~GameObject();

	short GetID() const;

	XMMATRIX GetModelMatrix();

	XMFLOAT3 GetPosition();
	void SetPosition(XMFLOAT3 position);

	Type GetType() const;

	bool IsVisible() const;
	void SetVisibility(bool visible);

	void SetRenderObjectID(int renderObjectID);
	int GetRenderObjectID() const;

	//Update object gamelogic
	void virtual Update();

	//Release object resources
	void virtual Release() = 0;
};

