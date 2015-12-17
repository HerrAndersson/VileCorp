#include "GameObject.h"

GameObject::GameObject()
{

}

GameObject::GameObject(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation,  AI::Vec2D tilePosition, Type type, RenderObject * renderObject)
{
	_ID = ID;
	_position = position;
	_rotation = rotation;
	_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	_tilePosition = tilePosition;
	_type = type;
	_renderObject = renderObject;
	CalculateMatrix();
	if (_renderObject->_isSkinned)
	{
		_animation = new Animation(_renderObject->_skeleton);
	}
}

GameObject::~GameObject()
{
	if (_renderObject->_isSkinned)
	{
		delete _animation;
	}
}

void GameObject::CalculateMatrix()
{
	//_objectMatrix = DirectX::XMMatrixAffineTransformation(
	//	DirectX::XMLoadFloat3(&_scale),
	//	DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0)),
	//	DirectX::XMQuaternionRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z),
	//	XMLoadFloat3(&_position)
	//	);

	//DO NOT TOUCH!!! //Haxxmaxxer
	_objectMatrix = DirectX::XMMatrixTranspose(
		DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&_scale)) * 
		DirectX::XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z) *
		DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&_position))
		);

}

unsigned short GameObject::GetID() const
{
	return _ID;
}

DirectX::XMMATRIX GameObject::GetMatrix() const
{

	return _objectMatrix;
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

AI::Vec2D GameObject::GetTilePosition() const
{
	return _tilePosition;
}

void GameObject::SetTilePosition(AI::Vec2D pos)
{
	_tilePosition = pos;
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

Animation * GameObject::GetAnimation() const
{
	return _animation;
}

void* GameObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void GameObject::operator delete(void* p)
{
	_mm_free(p);
}
