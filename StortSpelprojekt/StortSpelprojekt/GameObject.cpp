#include "GameObject.h"



GameObject::GameObject()
{}

GameObject::~GameObject()
{}

short GameObject::GetID() const
{
	return _ID;
}

Vec3 GameObject::GetPosition()
{
	return _position;
}

void GameObject::SetPosition(Vec3 position)
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

void GameObject::SetVisability(bool visible)
{
	_visible = visible;
}

RenderObject * GameObject::GetRenderObject() const
{
	return _renderObject;
}
