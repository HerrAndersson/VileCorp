#include "Pointlight.h"
using namespace DirectX;

namespace Renderer
{
	Pointlight::Pointlight(XMFLOAT3 position, float range, float intensity, int resolution, XMFLOAT3 color)
	{
		_position = position;
		_range = range;
		_intensity = intensity;
		_color = color;

		CreateIcoSphereVolume();
	}

	Pointlight::~Pointlight()
	{
	}

	void Pointlight::CreateIcoSphereVolume()
	{

	}

	void Pointlight::SetRange(float range)
	{
		_range = range;
	}

	void Pointlight::SetColor(XMFLOAT3 color)
	{
		_color = color;
	}

	void Pointlight::SetPosition(XMFLOAT3 position)
	{
		_worldMatrix = XMMatrixTranslation(_position.x, _position.y, _position.z);
	}

	XMMATRIX* Pointlight::GetWorldMatrix()
	{
		return &_worldMatrix;
	}

	XMFLOAT3 Pointlight::GetPosition() const
	{
		return _position;
	}

	XMFLOAT3 Pointlight::GetColor() const
	{
		return _color;
	}

	float Pointlight::GetRange() const
	{
		return _range;
	}

	float Pointlight::GetIntensity() const
	{
		return _intensity;
	}

	int Pointlight::GetVertexCount() const
	{
		return _vertexCount;
	}

	int Pointlight::GetVertexSize() const
	{
		return _vertexSize;
	}
}
