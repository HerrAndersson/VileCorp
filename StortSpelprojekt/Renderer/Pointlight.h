#pragma once
#define RENDERER_EXPORT __declspec(dllexport)
#include <DirectXMath.h>
#include <d3d11.h>

//Disable warning about DirectX XMFLOAT3/XMMATRIX etc
#pragma warning( disable: 4251 )

namespace Renderer
{
	class RENDERER_EXPORT Pointlight
	{

	private:

		DirectX::XMMATRIX   _worldMatrix;
		DirectX::XMFLOAT3	_position;
		DirectX::XMFLOAT3	_color;

		float				_intensity;
		float				_range;

		ID3D11Buffer*		_lightSphereVolume;
		int					_vertexCount;
		int					_vertexSize;

		void CreateIcoSphereVolume();

	public:

		Pointlight(DirectX::XMFLOAT3 position, float range, float intensity, int resolution, DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(0,0,0));
		virtual ~Pointlight();

		void SetRange(float range);
		void SetColor(DirectX::XMFLOAT3 color);
		void SetPosition(DirectX::XMFLOAT3 position);

		DirectX::XMMATRIX* GetWorldMatrix();
		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMFLOAT3 GetColor() const;
		float GetRange() const;
		float GetIntensity() const;
		int GetVertexCount() const;
		int GetVertexSize() const;

	};
}

