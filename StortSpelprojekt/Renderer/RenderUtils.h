#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <map>
#include "CommonUtils.h"

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

	void* Frame::operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void Frame::operator delete(void* p)
	{
		_mm_free(p);
	}
};

struct BoneFrames
{
	int _frameCount;
	std::vector<float> _frameTime;
	std::vector<Frame> _frames;
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
	System::Hitbox* _hitbox = nullptr;
	bool _isSkinned = false;
	bool DecrementUsers();
	std::string _skeletonName;
	Skeleton* _skeleton;
	int _vertexBufferSize, _toMesh;
	float _particleSpawnerPos[3], _iconPos[3];
	ID3D11Buffer* _vertexBuffer;
	std::string _name;
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
		if (_hitbox != nullptr)
		{
			delete _hitbox;
		}
	}
};

struct Texture
{
	HRESULT LoadTexture(ID3D11Device* device);
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
};

struct RenderObject
{
	System::Type _type = System::Type::FLOOR;
	Texture* _diffuseTexture = nullptr;
	Texture* _specularTexture = nullptr;
	Mesh* _mesh = nullptr;
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
		if (_mesh->DecrementUsers())
		{
			delete _mesh;
		}
	}
	bool operator==(const RenderObject& other) 
	{
		if (this->_diffuseTexture != nullptr && other._diffuseTexture != nullptr)
		{
			if (this->_diffuseTexture->_name != other._diffuseTexture->_name)
			{
				return false;
			}
		}
		else if (this->_diffuseTexture != other._diffuseTexture)
		{
			return false;
		}
		if (this->_mesh != nullptr && other._mesh != nullptr)
		{
			if (this->_mesh->_name != other._mesh->_name)
			{
				return false;
			}
		}
		else if (this->_mesh != other._mesh)
		{
			return false;
		}
		return true;
	}
	bool operator!=(const RenderObject& other) 
	{
		return !(*this == other);
	}
};