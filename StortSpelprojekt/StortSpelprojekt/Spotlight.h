#pragma once
#include <DirectXMath.h>

class Spotlight
{
private:

	DirectX::XMFLOAT3	_position;
	DirectX::XMFLOAT3	_rotation;
	DirectX::XMFLOAT3	_direction;
	DirectX::XMFLOAT3	_up;

	DirectX::XMMATRIX	_viewMatrix;
	DirectX::XMMATRIX	_projectionMatrix;

	void Update();

public:

	Spotlight(float nearClip, float farClip, float fov, int width, int height);
	~Spotlight();

	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void SetPositionAndRotation(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation);

	DirectX::XMFLOAT3 GetPosition()const;
	DirectX::XMFLOAT3 GetRotation()const;
	DirectX::XMFLOAT3 GetDirection() const;

	DirectX::XMMATRIX* GetViewMatrix();
	DirectX::XMMATRIX* GetProjectionMatrix();



	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

};

