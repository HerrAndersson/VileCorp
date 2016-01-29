#include "SecurityCamera.h"

void SecurityCamera::Rotate()
{
	if (_direction._x != 0 || _direction._y != 0)
	{
		if (_direction._x == 0)
		{
			_rotation.y = DirectX::XM_PIDIV2 * (_direction._y + 1);
		}
		else if (_direction._x == -1)
		{
			_rotation.y = DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4 * _direction._y;
		}
		else
		{
			_rotation.y = 3 * DirectX::XM_PIDIV2 - DirectX::XM_PIDIV4 * _direction._y;
		}
		CalculateMatrix();
	}
	ColorVisibleTiles({0,0,0});
	_visionCone->FindVisibleTiles(_tilePosition, _direction);
	ColorVisibleTiles({0,0,3});
	CheckVisibleTiles();
}

SecurityCamera::SecurityCamera()
	: GameObject()
{
	_tileMap = nullptr;
	_direction = {0,1};
	_visionRadius = 0;
	_visionCone = nullptr;
}

SecurityCamera::SecurityCamera(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject, const Tilemap * tileMap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{}

SecurityCamera::~SecurityCamera()
{
	delete[] _visionCone;
}

AI::Vec2D SecurityCamera::GetDirection()
{
	return AI::Vec2D();
}

void SecurityCamera::CheckVisibleTiles()
{
//	AI::Vec2D* visibleTiles = _visionCone->GetVisibleTiles();
	AI::Vec2D tile = {0,0};
	for (size_t i = 0; i < _visionCone->GetNrOfVisibleTiles(); i++)
	{
		tile = _visionCone->GetVisibleTile(i);
		if (_tileMap->IsEnemyOnTile(tile._x, tile._y))
		{

		}
	}
}

void SecurityCamera::Update(float deltaTime)
{}

void SecurityCamera::Release()
{}

void SecurityCamera::ColorVisibleTiles(DirectX::XMFLOAT3 color)
{}
