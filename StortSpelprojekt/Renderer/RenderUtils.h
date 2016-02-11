#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <map>

enum Type { FLOOR, WALL, LOOT, SPAWN, TRAP, CAMERA, GUARD, ENEMY, FURNITURE, NR_OF_TYPES/*Has to be last*/ };
const std::string MODEL_FOLDER_PATH = "Assets/Models/";
const std::wstring TEXTURE_FOLDER_PATH_W = L"Assets/Textures/";
const std::string LEVEL_FOLDER_PATH = "Assets/Levels/";
const std::string ANIMATION_FOLDER_PATH = "Assets/Animations/";
const std::string TILESET_FOLDER_PATH = "Assets/Tilesets/";

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
	ID3D11Buffer* _vertexBuffer;
	bool _meshLoaded;
	bool _isSkinned = false;
	bool DecrementUsers();
	short _activeUsers = 0;
	Skeleton* _skeleton;
	int _vertexBufferSize, _toMesh;
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
	}
};

struct Texture
{
	HRESULT LoadTexture(ID3D11Device* device);
	bool DecrementUsers();
	short _activeUsers = 0;
	bool _loaded = false;
	std::string _name;
	ID3D11ShaderResourceView* _data = nullptr;
};

struct RenderObject
{
	Type _type = Type::FLOOR;
	Texture* _diffuseTexture = nullptr;
	Texture* _specularTexture = nullptr;
	Mesh* _mesh;
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
		_mesh->DecrementUsers();
	}
};