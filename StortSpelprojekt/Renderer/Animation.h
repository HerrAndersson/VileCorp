#pragma once

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>
#include "RenderUtils.h"
#include "CommonUtils.h"

using namespace DirectX;

class __declspec(dllexport) Animation
{
private:

	XMMATRIX Interpolate(unsigned int boneID, int action);

	bool _frozen;
	float _time;
	float _currTime;
	float _nextTime;
	float _lerpPercent;
	unsigned _lastFrame, _boneCount;
	XMVECTOR _zeroVector = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMMATRIX* toRootTransforms;
	XMMATRIX* toParentTransforms;
	XMMATRIX* finalTransforms;
	std::vector<XMFLOAT4X4> finalFloats;
	Skeleton* _skeleton;
	float _animTime;
	int _currentCycle, _currentAction;

public:
	Animation(Skeleton* skeleton);
	~Animation();

	void Update(float time);
	void SetActionAsCycle(int action, bool reset);
	void Freeze(bool freeze);
	void PlayAction(int action);
	XMMATRIX* GetTransforms()
	{
		return finalTransforms;
	}
	std::vector<XMFLOAT4X4>* GetFloats()
	{
		return &finalFloats;
	}
	int GetBoneCount() const
	{
		return _boneCount;
	} 
};