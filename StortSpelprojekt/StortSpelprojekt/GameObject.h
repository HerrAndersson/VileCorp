#pragma once

/*
GameObject class
Base interface for specialized GameObjects.

Contains an unique ID for identification, a world position and a enum Type (TODO come up with a good system for types, (string/char*, or maybe new hierchical classes?)
If the object doesn't need a _renderObject, set it to nullptr.
If the object has a renderObject but is out of sight _visibility will be false.
*/

//placeholder
struct Vec3
{
	float _posX, _posY, _posZ;
};

//placeholder
struct RenderObject
{
	//Matrices, shaderinfo, buffers and stuff. Structure will probably be in the renderengine
};



//TODO look into static casting to find out what part of the objecthiearchy the object lies in
//struct Type
//{
//	string _name;
//	string _parent;
//};

//Preliminary solution
enum Type {UNIT, STRUCTURE, TILE, TRAP, TRIGGER };

class GameObject
{
protected:
	unsigned short _ID;
	Vec3 _position;
	//Vec3 _direction;
	Type _type;
	bool _visible; // OBS
	RenderObject* _renderObject;

public:
	GameObject();
	~GameObject();

	short GetID() const;

	//TODO change Vec3 to XMVECTOR or other vectorclass
	Vec3 GetPosition();
	void SetPosition(Vec3 position);

	Type GetType() const;

	bool IsVisible() const;
	void SetVisability(bool visible);

	RenderObject* GetRenderObject() const;

	//Update object gamelogic
	void virtual Update() = 0;

	//Release object resources
	void virtual Release() = 0;
};

