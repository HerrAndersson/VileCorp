  #pragma once
#include <DirectXMath.h>
#include "RenderUtils.h"
#include "AIUtil.h"
#include "Animation.h"

/*
GameObject class
Base interface for specialized GameObjects.

Contains an unique ID for identification, a world position and a enum Type (TODO come up with a good system for types, (string/char*, or maybe new hierchical classes?)
If the object doesn't need a _renderObject, set it to nullptr.
If the object has a renderObject but is out of sight _visibility will be false.
*/

enum PickUpState{ONTILE, HELD, PICKINGUP, DROPPING};
class GameObject
{
protected:

	unsigned short _ID;
	DirectX::XMMATRIX _objectMatrix;
	DirectX::XMFLOAT3 _position;
	DirectX::XMFLOAT3 _rotation;
	DirectX::XMFLOAT3 _scale;
	DirectX::XMFLOAT3 _colorOffset;
	AI::Vec2D _tilePosition;
	Type _type;
	unsigned int _subType;
	bool _visible;
	bool _active;
	RenderObject* _renderObject;
	Animation* _animation = nullptr;

	PickUpState _pickUpState;

	void CalculateMatrix();

public:

	GameObject();
	//Type might not be necessary, depending on whether subclasses can correspond to one type or many.
	GameObject(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0,0,0));
	virtual ~GameObject();

	unsigned short GetID() const;

	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMFLOAT3 GetRotation() const;
	DirectX::XMFLOAT3 GetScale() const;
	DirectX::XMMATRIX* GetMatrix();
	DirectX::XMFLOAT3 GetColorOffset() const;

	void SetPosition(const DirectX::XMFLOAT3& position);
	void SetRotation(const DirectX::XMFLOAT3& rotation);
	void SetScale(const DirectX::XMFLOAT3& scale);
	void SetColorOffset(const DirectX::XMFLOAT3& colorOffset);

	void AddColorOffset(const DirectX::XMFLOAT3& colorOffset);

	void Translate(const DirectX::XMFLOAT3& offset);
	void Scale(const DirectX::XMFLOAT3& scale);
	void Rotate(const DirectX::XMFLOAT3& rotate);

	AI::Vec2D GetTilePosition()const;
	virtual void SetTilePosition(AI::Vec2D pos);
	Type GetType() const;
	unsigned int GetSubType() const;

	bool IsVisible() const;
	bool IsActive() const;
	void SetActive(bool active);
	void SetVisibility(bool visible);

	RenderObject* GetRenderObject() const;
	Animation* GetAnimation() const;

	void SetPickUpState( PickUpState state);
	PickUpState GetPickUpState()const;

	//Update object gamelogic
	void virtual Update(float deltaTime) = 0;

	void virtual Release() = 0;

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);
};

