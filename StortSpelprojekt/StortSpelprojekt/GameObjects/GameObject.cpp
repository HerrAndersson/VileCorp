#include "GameObject.h"

GameObject::GameObject(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation,  AI::Vec2D tilePosition, System::Type type, RenderObject * renderObject, System::SoundModule* soundModule, DirectX::XMFLOAT3 colorOffset, int subType )
{
	_ID = ID;
	_position = position;
	_rotation = rotation;
	_colorOffset = colorOffset;
	_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	_tilePosition = tilePosition;
	_type = type;
	_renderObject = renderObject;
	_pickUpState = ONTILE;
	_visible = true;
	_subType = subType;
	_active = true;
	_soundModule = soundModule;
	_animation = nullptr;

	CalculateMatrix();
}

GameObject::~GameObject()
{
}

void GameObject::CalculateMatrix()
{
	_objectMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&_scale)) 
			      * DirectX::XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z) 
		          * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&_position));
}

unsigned short GameObject::GetID() const
{
	return _ID;
}

DirectX::XMMATRIX* GameObject::GetMatrix()
{
	return &_objectMatrix;
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

void GameObject::Translate(const DirectX::XMFLOAT3 & offset)
{
	_position.x += offset.x;
	_position.y += offset.y;
	_position.z += offset.z;
	CalculateMatrix();
}

void GameObject::Scale(const DirectX::XMFLOAT3& scale)
{
	_scale.x += scale.x;
	_scale.y += scale.y;
	_scale.z += scale.z;
	CalculateMatrix();
}

void GameObject::Rotate(const DirectX::XMFLOAT3& rotate)
{
	_rotation.x = rotate.x;
	_rotation.y = rotate.y;
	_rotation.z = rotate.z;
	CalculateMatrix();
}
AI::Vec2D GameObject::GetTilePosition() const
{
	return _tilePosition;
}

AI::Vec2D GameObject::GetDirection() const
{
	return _direction;
}

void GameObject::SetTilePosition(AI::Vec2D pos)
{
	_tilePosition = pos;
}

void GameObject::SetDirection(AI::Vec2D dir)
{
	_direction = dir;
}

System::Type GameObject::GetType() const
{
	return _type;
}

bool GameObject::IsCenteredOnTile(AI::Vec2D tile) const
{
	return abs(_position.x - tile._x) < TILE_EPSILON && abs(_position.z - tile._y) < TILE_EPSILON;
}

unsigned int GameObject::GetSubType() const
{
	return _subType;
}

bool GameObject::IsVisible() const
{
	return _visible;
}

bool GameObject::IsActive() const
{
	return _active;
}

//void GameObject::SetActive(bool active)
//{
//	_active = active;
//}

void GameObject::SetVisibility(bool visible)
{
	_visible = visible;
}

bool GameObject::InRange(AI::Vec2D pos) const
{
	bool result = false;
	AI::Vec2D tempPos = _tilePosition;
	if (pos == tempPos)
	{
		result = true;
	}
	for (int i = 0; i < 8 && !result; i++)
	{
		if (pos == tempPos + AI::NEIGHBOUR_OFFSETS[i])
		{
			result = true;
		}
	}
	return result;
}

RenderObject * GameObject::GetRenderObject() const
{
	return _renderObject;
}

Animation * GameObject::GetAnimation() const
{
	return _animation;
}
	
	return nullptr;

void GameObject::SetPickUpState(PickUpState state)
{
	_pickUpState = state;
}

PickUpState GameObject::GetPickUpState()const
{
	return _pickUpState;
}

DirectX::XMFLOAT3 GameObject::GetColorOffset() const
{
	return _colorOffset;
}

void GameObject::SetColorOffset(const DirectX::XMFLOAT3& colorOffset)
{
	_colorOffset = colorOffset;
}

void GameObject::AddColorOffset(const DirectX::XMFLOAT3& colorOffset)
{
	_colorOffset.x += colorOffset.x;
	_colorOffset.y += colorOffset.y;
	_colorOffset.z += colorOffset.z;
}

void* GameObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void GameObject::operator delete(void* p)
{
	_mm_free(p);
}

bool GameObject::operator<(const GameObject& other)
{
	return (this->_renderObject < other._renderObject);
}

int GameObject::GetAnimLength(int layer)
{
	if (_animation != nullptr)
	{
		return _animation->GetLength(layer);
	}
	else
	{
		return 0;
	}
}