#include "GameObject.h"



GameObject::GameObject()
{}

GameObject::GameObject(unsigned short ID, Vec3 position, RenderObject * renderObject)
{
	_ID = ID;
	_position = position;
	_renderObject = renderObject;
}

GameObject::~GameObject()
{}

short GameObject::GetID() const
{
	return _ID;
}

XMMATRIX GameObject::GetModelMatrix()
{
	return XMMatrixAffineTransformation(
		XMLoadFloat3(&_scale),
		XMLoadFloat3(&XMFLOAT3(0, 0, 0)),
		XMQuaternionRotationRollPitchYaw(_direction.x, _direction.y, _direction.z),
		XMLoadFloat3(&_position)
		);
}

XMFLOAT3 GameObject::GetPosition()
{
	return _position;
}

void GameObject::SetPosition(XMFLOAT3 position)
{
	_position = position;
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

void GameObject::SetRenderObjectID(int renderObjectID)
{
	_renderObjectID = renderObjectID;
}

int GameObject::GetRenderObjectID() const
{
	return _renderObjectID;
}
