#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <map>
#include "CommonUtils.h"
#include "stdafx.h"

static const DirectX::XMFLOAT3 AMBIENT_LIGHT_NIGHT = DirectX::XMFLOAT3(0.14f, 0.15f, 0.2f);
static const DirectX::XMFLOAT3 AMBIENT_LIGHT_DAY = DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f);

struct Bone
{
	int _parent;
	DirectX::XMFLOAT4X4 _inverseBindpose;
};

struct Frame
{
	DirectX::XMVECTOR _translation;
	DirectX::XMVECTOR _rotation;
	DirectX::XMVECTOR _scale;

	static void* Frame::operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	static void Frame::operator delete(void* p)
	{
		_mm_free(p);
	}
};

struct BoneFrames
{
	int _frameCount;
	std::vector<float> _frameTime;
	Frame* _frames;

	~BoneFrames()
	{
		_aligned_free(_frames);
	}
};

struct Action
{
	std::vector<BoneFrames> _bones;
};

struct Skeleton
{
	std::string _name;
	std::vector<int> _parents;
	DirectX::XMMATRIX* _bindposes;
	std::vector<Action> _actions;
	~Skeleton()
	{
		_aligned_free(_bindposes);
	}
};

struct WeightedVertex
{
	unsigned int _boneIndices[4];
	DirectX::XMFLOAT3 _position;
	DirectX::XMFLOAT3 _normal;
	DirectX::XMFLOAT2 _uv;
	float _boneWeights[4];
};

struct Vertex
{
	DirectX::XMFLOAT3 _position;
	DirectX::XMFLOAT3 _normal;
	DirectX::XMFLOAT2 _uv;
};

struct PointlightData
{
	unsigned char _bone;
	float _range, _intensity;
	DirectX::XMFLOAT3 _pos, _col;
};

struct SpotlightData
{
	unsigned char _bone;
	float _intensity, _angle, _range;
	DirectX::XMFLOAT3 _pos, _color, _direction;
	bool _shadowsEnabled = true;
};

struct Point
{
	DirectX::XMFLOAT3 _position;
	int _boneIndices[4];
	float _boneWeights[4];
};

struct Mesh
{
	short _activeUsers = 0;
	bool _meshLoaded;
	bool _isSkinned = false;
	int _vertexBufferSize, _toMesh;
	float _particleSpawnerPos[3], _iconPos[3];
	System::Hitbox* _hitbox = nullptr;
	Skeleton* _skeleton;
	ID3D11Buffer* _vertexBuffer;
	std::string _name;
	std::string _skeletonName;
	std::vector<PointlightData> _pointLights;
	std::vector<SpotlightData> _spotLights;
	~Mesh()
	{
		_pointLights.clear();
		_spotLights.clear();
		if (_meshLoaded && _vertexBuffer != nullptr)
		{
			_vertexBuffer->Release();
		}
		delete _hitbox;
	}

	bool DecrementUsers()
	{
		_activeUsers--;
		return (!_activeUsers);
	}
};

struct Texture
{
	HRESULT LoadTexture(ID3D11Device* device);
	int _id = 0;
	short _activeUsers = 0;
	bool _loaded = false;
	std::string _name;
	ID3D11ShaderResourceView* _data = nullptr;
	void DecrementUsers()
	{
		_activeUsers--;
		if (!_activeUsers)
		{
			_loaded = false;
			_data->Release();
		}
	}
	void IncrementUsers()
	{
		_activeUsers++;
	}
};

struct RenderObject
{
	Texture* _diffuseTexture = nullptr;
	Texture* _specularTexture = nullptr;
	Mesh* _mesh = nullptr;
	int _id;
	~RenderObject()
	{
		if (_diffuseTexture != nullptr)
		{
			_diffuseTexture->DecrementUsers();
		}
		if (_specularTexture != nullptr)
		{
			_specularTexture->DecrementUsers();
		}
		if (_mesh && _mesh->DecrementUsers())
		{
			delete _mesh;
		}
	}

	bool operator<(const RenderObject& other)
	{
		return (this->_diffuseTexture->_id < other._diffuseTexture->_id);
	}

	bool operator==(const RenderObject& other) 
	{
		return _id == other._id;
	}

	bool operator!=(const RenderObject& other) 
	{
		return !(*this == other);
	}
};