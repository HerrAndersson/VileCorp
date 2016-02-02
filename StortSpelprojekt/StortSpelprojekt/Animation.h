#pragma once

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>
#include "RenderUtils.h"

class Animation
{
private:

	DirectX::XMMATRIX Interpolate(unsigned int boneID, int action);

	float _time;
	float _currTime;
	float _nextTime;
	float _lerpPercent;
	int _lastFrame;
	DirectX::XMVECTOR _zeroVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	std::vector<DirectX::XMMATRIX> toRootTransforms;
	std::vector<DirectX::XMMATRIX> toParentTransforms;
	std::vector<DirectX::XMMATRIX> finalTransforms;
	Skeleton* _skeleton;
	float _animTime;
	int _currentCycle, _currentAction;

public:
	Animation(Skeleton* skeleton);
	~Animation();

	void Update(float time);
	void SetActionAsCycle(int action);
	void PlayAction(int action);
	std::vector<DirectX::XMMATRIX>* GetTransforms()
	{
		return &finalTransforms;
	}

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);
};