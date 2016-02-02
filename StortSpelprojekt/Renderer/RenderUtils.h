#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <map>

enum Type { FLOOR, WALL, LOOT, SPAWN, TRAP, GUARD, ENEMY, NR_OF_TYPES/*Has to be last*/ };
const std::string MODEL_FOLDER_PATH = "Assets/Models/";
const std::wstring TEXTURE_FOLDER_PATH_W = L"Assets/Textures/";
const std::string LEVEL_FOLDER_PATH = "Assets/Levels/";

struct Bone
{
	int _parent;
	DirectX::XMFLOAT4X4 _inverseBindpose;
};

struct Frame
{
	DirectX::XMFLOAT3 _translation;
	DirectX::XMFLOAT4 _rotation;
	DirectX::XMFLOAT3 _scale;
};

struct BoneFrames
{
	std::vector<Frame> _frames;
};

struct Action
{
	std::vector<float> _frameTime;
	std::vector<BoneFrames> _bones;
};

struct Skeleton
{
	std::string _name;
	std::vector<Bone> _skeleton;
	std::vector<Action> _actions;
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
	int _vertexBufferSize, _toMesh;
	std::vector<PointlightData> _pointLights;
	std::vector<SpotlightData> _spotLights;
	~Mesh()
	{
		_pointLights.clear();
		_spotLights.clear();
	}
};

struct Texture
{
	HRESULT LoadTexture(ID3D11Device* device);
	bool DecrementUsers();
	bool _loaded = false;
	short _activeUsers = 0;
	std::wstring _filePath;
	std::string _name;
	ID3D11ShaderResourceView* _data = nullptr;
};

struct RenderObject
{
	Type _type = Type::FLOOR;
	bool _meshLoaded, _toUnload;
	bool _isSkinned = false;
	std::string _name;
	std::string _skeletonName;
	Skeleton* _skeleton;
	float _diffuse[4], _specular[4];
	Texture* _diffuseTexture = nullptr;
	Texture* _specularTexture = nullptr;
	Mesh _mesh;
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
		if (_meshLoaded && _mesh._vertexBuffer != nullptr)
		{
			_mesh._vertexBuffer->Release();
		}
	}
};