#pragma once

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>
#include "RenderUtils.h"
#include "CommonUtils.h"

using namespace DirectX;

//Disable warning about dll-interface
#pragma warning( disable: 4251 )

class __declspec(dllexport) Animation
{
private:

	XMVECTOR _zeroVector = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMMATRIX Interpolate(unsigned int boneID, int action);

	bool _frozen;
	float _time;
	float _currTime;
	float _nextTime;
	float _lerpPercent;
	unsigned _lastFrame, _boneCount;
	std::vector<XMMATRIX> _toRootTransforms;
	std::vector<XMMATRIX> _toParentTransforms;
	std::vector<XMMATRIX> _finalTransforms;
	Skeleton* _skeleton;
	float _animTime;
	int _currentCycle, _currentAction;
	bool _inactive;
	bool _lastFrameRender;
	bool _isFinished;
	std::vector<int> _length;

public:
	Animation(Skeleton* skeleton, bool firstFrame = false, bool frozen = false);
	~Animation();

	void Update(float time);
	void SetActionAsCycle(int action, bool reset = false);
	void Freeze(bool freeze);
	XMMATRIX* GetTransforms();
	void PlayAction(int action, bool freeze = false, bool lastFrame = false, bool reset = false);
	int GetBoneCount() const;
	bool GetisFinished();
	float GetLength(int animation);

	static void* Animation::operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	static void Animation::operator delete(void* p)
	{
		_mm_free(p);
	}
};