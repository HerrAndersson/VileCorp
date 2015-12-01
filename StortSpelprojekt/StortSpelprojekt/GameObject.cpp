#include "GameObject.h"

GameObject::GameObject()
{}

GameObject::GameObject(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation,  AI::Vec2D tilePosition, Type type, RenderObject * renderObject)
{
	_ID = ID;
	_position = position;
	_rotation = rotation;
	_scale = DirectX::XMFLOAT3(1, 1, 1);
	_tilePosition = tilePosition;
	_type = type;
	_renderObject = renderObject;
	CalculateMatrix();
}

GameObject::~GameObject()
{}

void GameObject::CalculateMatrix()
{
	_objectMatrix = DirectX::XMMatrixAffineTransformation(
		DirectX::XMLoadFloat3(&_scale),
		DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0)),
		DirectX::XMQuaternionRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z),
		XMLoadFloat3(&_position)
		);
}

short GameObject::GetID() const
{
	return _ID;
}

DirectX::XMFLOAT3 GameObject::GetPosition() const
{
	return _position;
}

DirectX::XMFLOAT3 GameObject::GetRotation() const
{
	return _rotation;
}

DirectX::XMFLOAT3 GameObject::GetScale() const
{
	return _scale;
}

void GameObject::SetPosition(const DirectX::XMFLOAT3 & position)
{
	_position = position;
	CalculateMatrix();
}

void GameObject::SetRotation(const DirectX::XMFLOAT3 & rotation)
{
	_rotation = rotation;
	CalculateMatrix();
}

void GameObject::SetScale(const DirectX::XMFLOAT3 & scale)
{
	_scale = scale;
	CalculateMatrix();
}

Type GameObject::GetType() const
{
	return _type;
}

bool GameObject::IsVisible() const
{
	return _visible;
}

void GameObject::SetVisibility(bool visible)
{
	_visible = visible;
}

RenderObject * GameObject::GetRenderObject() const
{
	return _renderObject;
}
