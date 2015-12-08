#pragma once

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>
#include "RenderUtils.h"

class Animation
{
private:

	void Interpolate(float time, unsigned int boneID, unsigned int layerID, DirectX::XMFLOAT4X4& matrix);

	float _time;
	float _currTime;
	float _nextTime;
	float _lerpPercent;
	DirectX::XMVECTOR _zeroVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	std::vector<DirectX::XMFLOAT4X4> toRootTransforms;
	std::vector<DirectX::XMFLOAT4X4> toParentTransforms;
	RenderObject* _renderObject;

public:
	Animation();
	~Animation();

	void Update(float time, unsigned int layerID, RenderObject* renderObject);
};