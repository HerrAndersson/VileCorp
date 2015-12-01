#pragma once
#include <DirectXMath.h>
#include "RenderUtils.h"
#include "AIUtil.h"

/*
GameObject class
Base interface for specialized GameObjects.

Contains an unique ID for identification, a world position and a enum Type (TODO come up with a good system for types, (string/char*, or maybe new hierchical classes?)
If the object doesn't need a _renderObject, set it to nullptr.
If the object has a renderObject but is out of sight _visibility will be false.
*/

enum Type {UNIT, WALL, FLOOR, TRAP, TRIGGER, LOOT };

class GameObject
{
protected:
	unsigned short _ID;
	DirectX::XMMATRIX _objectMatrix;
	DirectX::XMFLOAT3 _position;
	DirectX::XMFLOAT3 _rotation;
	DirectX::XMFLOAT3 _scale;
	AI::Vec2D _tilePosition;
	//Vec2i _direction;
	Type _type;
	bool _visible; // OBS
	RenderObject* _renderObject;

	void CalculateMatrix();

public:
	GameObject();
	//Type might not be necessary, depending on whether subclasses can correspond to one type or many.
	GameObject(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject);
	~GameObject();

	short GetID() const;

	//TODO change Vec3 to XMVECTOR or other vectorclass - Zache/Marcus
	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMFLOAT3 GetRotation() const;
	DirectX::XMFLOAT3 GetScale() const;
	DirectX::XMMATRIX GetObjectMatrix()const;

	void SetPosition(const DirectX::XMFLOAT3& position);
	void SetRotation(const DirectX::XMFLOAT3& rotation);
	void SetScale(const DirectX::XMFLOAT3& scale);

	AI::Vec2D GetTilePosition()const;
	void SetTilePosition(AI::Vec2D pos);
	Type GetType() const;

	bool IsVisible() const;
	void SetVisibility(bool visible);

	RenderObject* GetRenderObject() const;

	//Update object gamelogic
	void virtual Update() = 0;
};

