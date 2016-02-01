#include"Animation.h"

Animation::Animation(Skeleton* skeleton)
{
	_skeleton = skeleton;
	toRootTransforms.resize(_skeleton->_skeleton.size());
	toParentTransforms.resize(_skeleton->_skeleton.size());
	finalTransforms.resize(_skeleton->_skeleton.size());
	for (unsigned int i = 0; i < finalTransforms.size(); i++)
	{
		DirectX::XMStoreFloat4x4(&finalTransforms[i], DirectX::XMMatrixIdentity());
	}
	_animTime = 0.0f;
	_currentAction = -1;
	_currentCycle = 0;
}

Animation::~Animation()
{

}

void Animation::Update(float time)
{
	_animTime += time/1000;
	if (_currentAction != -1)
	{
		if (_skeleton->_actions[_currentAction]._bones[0]._frameTime.back() < _animTime)
		{
			_currentAction = -1;
			_animTime = 0.0f;
		}
		else
		{
			for (unsigned int i = 0; i < _skeleton->_skeleton.size(); i++)
			{
				toParentTransforms[i] = Interpolate(i, _currentAction);
			}
		}
	}
	if (_currentAction == -1)
	{
		if (_skeleton->_actions[_currentCycle]._bones[0]._frameTime.back() < _animTime)
		{
			_animTime -= _skeleton->_actions[_currentCycle]._bones[0]._frameTime.back();
		}
		for (unsigned int i = 0; i < _skeleton->_skeleton.size(); i++)
		{
			toParentTransforms[i] = Interpolate(i, _currentCycle);
		}
	}

	toRootTransforms[0] = toParentTransforms[0];

	for (unsigned int i = 1; i < _skeleton->_skeleton.size(); i++)
	{
		// Current bone transform relative to its parent
		DirectX::XMMATRIX toParent = DirectX::XMLoadFloat4x4(&toParentTransforms[i]);
		DirectX::XMMATRIX parentToRoot = DirectX::XMLoadFloat4x4(&toRootTransforms[_skeleton->_skeleton[i]._parent]);
		DirectX::XMMATRIX toRoot = toParent * parentToRoot;
		DirectX::XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	for (unsigned int i = 0; i < _skeleton->_skeleton.size(); i++)
	{
		DirectX::XMMATRIX offset = DirectX::XMLoadFloat4x4(&_skeleton->_skeleton[i]._inverseBindpose);
		DirectX::XMMATRIX toRoot = DirectX::XMLoadFloat4x4(&toRootTransforms[i]);
		DirectX::XMStoreFloat4x4(&finalTransforms[i], DirectX::XMMatrixTranspose(offset * toRoot));
	}
}

void Animation::SetActionAsCycle(int action)
{
	_animTime = 0.0f;
	_currentCycle = action;
}

void Animation::PlayAction(int action)
{
	_animTime = 0.0f;
	_currentAction = action;
}

DirectX::XMFLOAT4X4 Animation::Interpolate(unsigned int boneID, int action)
{
	DirectX::XMFLOAT4X4 matrix;
	_lastFrame = _skeleton->_actions[action]._bones[boneID]._frameTime.size() - 1;
	if (_animTime <= _skeleton->_actions[action]._bones[boneID]._frameTime[0])
	{
		DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&_skeleton->_actions[action]._bones[boneID]._frames[0]._scale);
		DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&_skeleton->_actions[action]._bones[boneID]._frames[0]._translation);
		DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&_skeleton->_actions[action]._bones[boneID]._frames[0]._rotation);

		DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixAffineTransformation(s, _zeroVector, q, p));
	}
	
	else if (_animTime >= _skeleton->_actions[action]._bones[0]._frameTime[_lastFrame])
	{
		DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&_skeleton->_actions[action]._bones[boneID]._frames[_lastFrame]._scale);
		DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&_skeleton->_actions[action]._bones[boneID]._frames[_lastFrame]._translation);
		DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&_skeleton->_actions[action]._bones[boneID]._frames[_lastFrame]._rotation);

		DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixAffineTransformation(s, _zeroVector, q, p));
	}

	else
	{
		for (unsigned int i = 0; i <= _skeleton->_actions[action]._bones[boneID]._frameTime.size(); i++)
		{
			_currTime = _skeleton->_actions[action]._bones[boneID]._frameTime[i];
			_nextTime = _skeleton->_actions[action]._bones[boneID]._frameTime[i + 1];
			if (_animTime >= _currTime && _animTime <= _nextTime)
			{
				_lerpPercent = (_animTime - _currTime) / (_nextTime - _currTime);

				DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&_skeleton->_actions[action]._bones[boneID]._frames[i]._scale);
				DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&_skeleton->_actions[action]._bones[boneID]._frames[i + 1]._scale);

				DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&_skeleton->_actions[action]._bones[boneID]._frames[i]._translation);
				DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&_skeleton->_actions[action]._bones[boneID]._frames[i + 1]._translation);

				DirectX::XMVECTOR q0 = DirectX::XMLoadFloat4(&_skeleton->_actions[action]._bones[boneID]._frames[i]._rotation);
				DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&_skeleton->_actions[action]._bones[boneID]._frames[i + 1]._rotation);

				DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, _lerpPercent);
				DirectX::XMVECTOR p = DirectX::XMVectorLerp(p0, p1, _lerpPercent);
				DirectX::XMVECTOR q = DirectX::XMQuaternionSlerp(q0, q1, _lerpPercent);
				
				DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixAffineTransformation(s, _zeroVector, q, p));

				break;
			}
		}
	}
	return matrix;
}

void* Animation::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void Animation::operator delete(void* p)
{
	_mm_free(p);
}