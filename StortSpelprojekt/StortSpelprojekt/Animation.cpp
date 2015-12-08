#include"Animation.h"

Animation::Animation()
{

}

Animation::~Animation()
{

}

void Animation::Update(float time, unsigned int layerID, RenderObject* renderObject)
{
	//_renderObject = renderObject;

	//for (int i = 0; i < skelHead.nrOfBones; i++)
	//{
	//	Interpolate(time, i, layerID, toParentTransforms[i]);
	//}

	//toRootTransforms[0] = toParentTransforms[0];

	//for (int i = 1; i < skelHead.nrOfBones; i++)
	//{
	//	// Current bone transform relative to its parent
	//	DirectX::XMMATRIX toParent = DirectX::XMLoadFloat4x4(&toParentTransforms[i]);
	//	DirectX::XMMATRIX parentToRoot = DirectX::XMLoadFloat4x4(&toRootTransforms[boneData.parent[i]]);
	//	DirectX::XMMATRIX toRoot = toParent * parentToRoot;
	//	DirectX::XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	//}

	//for (int i = 0; i < skelHead.nrOfBones; i++)
	//{
	//	DirectX::XMMATRIX offset = DirectX::XMLoadFloat4x4(&boneData.bindPose[i]);
	//	DirectX::XMMATRIX toRoot = DirectX::XMLoadFloat4x4(&toRootTransforms[i]);
	//	DirectX::XMStoreFloat4x4(&finalTransforms[i], offset * toRoot);
	//}
}

void Animation::Interpolate(float time, unsigned int boneID, unsigned int layerID, DirectX::XMFLOAT4X4& matrix)
{
	//if (time <= animLayer[layer].times[0])
	//{
	//	DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&animLayer[layer].bones[boneID].tranform[0].scale);
	//	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&animLayer[layer].bones[boneID].tranform[0].translation);
	//	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&animLayer[layer].bones[boneID].tranform[0].rotation);

	//	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixAffineTransformation(s, _zeroVector, q, p));
	//}

	//else if (time >= animLayer[layer].times[animLayer[layer].nrOfKeys - 1])
	//{
	//	DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&animLayer[layer].bones[boneID].tranform[animLayer[layer].nrOfKeys - 1].scale);
	//	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&animLayer[layer].bones[boneID].tranform[animLayer[layer].nrOfKeys - 1].translation);
	//	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&animLayer[layer].bones[boneID].tranform[animLayer[layer].nrOfKeys - 1].rotation);

	//	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixAffineTransformation(s, _zeroVector, q, p));
	//}

	//else
	//{
	//	for (int i = 0; i <= animLayer[layer].nrOfKeys; i++)
	//	{
	//		_currTime = animLayer[layer].times[i];
	//		_nextTime = animLayer[layer].times[i + 1];
	//		if (time >= _currTime && time <= _nextTime)
	//		{
	//			_lerpPercent = (time - _currTime) / (_nextTime - _currTime);


	//			DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&animLayer[layer].bones[boneID].tranform[i].scale);
	//			DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&animLayer[layer].bones[boneID].tranform[i + 1].scale);

	//			DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&animLayer[layer].bones[boneID].tranform[i].translation);
	//			DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&animLayer[layer].bones[boneID].tranform[i + 1].translation);

	//			DirectX::XMVECTOR q0 = DirectX::XMLoadFloat4(&animLayer[layer].bones[boneID].tranform[i].rotation);
	//			DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&animLayer[layer].bones[boneID].tranform[i + 1].rotation);

	//			DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, _lerpPercent);
	//			DirectX::XMVECTOR p = DirectX::XMVectorLerp(p0, p1, _lerpPercent);
	//			DirectX::XMVECTOR q = DirectX::XMQuaternionSlerp(q0, q1, _lerpPercent);
	//			
	//			DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixAffineTransformation(s, _zeroVector, q, p));

	//			break;
	//		}
	//	}
	//}
}