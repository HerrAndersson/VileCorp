#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "RenderUtils.h"

#define RENDERER_EXPORT __declspec(dllexport)

//Disable warning about DirectX XMFLOAT3/XMMATRIX etc
#pragma warning( disable: 4251 )

namespace Renderer
{
	class RENDERER_EXPORT Spotlight
	{
	private:

		DirectX::XMMATRIX	_viewMatrix;
		DirectX::XMMATRIX	_projectionMatrix;
		DirectX::XMMATRIX   _worldMatrix;

	DirectX::XMFLOAT3	_position;
	DirectX::XMFLOAT3	_rotation;
	DirectX::XMFLOAT3	_direction;
	DirectX::XMFLOAT3	_up;

	float				_angle;
	float				_intensity;
	float				_range;
	bool				_active;
	unsigned char		_bone;

	DirectX::XMFLOAT3	_color;

		int					_vertexCount;
		int					_vertexSize;
	ID3D11Buffer*		_lightConeVolume;

	void Update();

public:

	Spotlight(ID3D11Device* device, SpotlightData lightdata, int width, int height, float nearClip, float farClip, int resolution = 72);
	~Spotlight();

	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void SetPositionAndRotation(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation);
	void SetPositionAndRotation(DirectX::XMMATRIX &matrix);

	void SetIntensity(float intensity);
	void SetRange(float range);
	void SetColor(DirectX::XMFLOAT3 color);

	float GetIntensity() const;
	float GetRange() const;
	float GetAngle() const;
		int GetVertexCount() const;
		int GetVertexSize() const;

	unsigned char GetBone();
	bool IsActive() const;
	void SetActive(bool active);

	DirectX::XMFLOAT3 GetColor() const;
	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMFLOAT3 GetRotation() const;
	DirectX::XMFLOAT3 GetDirection() const;

	DirectX::XMMATRIX* GetWorldMatrix();
	DirectX::XMMATRIX* GetViewMatrix();
	DirectX::XMMATRIX* GetProjectionMatrix();

	ID3D11Buffer* GetVolumeBuffer() const;

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

	};
}

