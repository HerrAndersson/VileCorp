#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <DirectXMath.h>

struct WeightedVertex
{
	DirectX::XMFLOAT3 _position;
	DirectX::XMFLOAT3 _normal;
	DirectX::XMFLOAT2 _uv;
	int _boneIndices[4];
	float _boneWeights[4];
};

struct Vertex
{
	DirectX::XMFLOAT3 _position;
	DirectX::XMFLOAT3 _normal;
	DirectX::XMFLOAT2 _uv;
};

struct PointLight
{
	float _pos[3], _col[3], _intensity;
};

struct SpotLight
{
	float _pos[3], _col[3], _intensity, _angle, _direction[3];
};

struct Point
{
	DirectX::XMFLOAT3 _position;
	int _boneIndices[4];
	float _boneWeights[4];
};

struct Mesh
{
	~Mesh()
	{
		_pointLights.clear();
		_spotLights.clear();
	}
	std::string _name;
	ID3D11Buffer* _vertexBuffer;
	int _vertexBufferSize, _toMesh;
	std::vector<PointLight> _pointLights;
	std::vector<SpotLight> _spotLights;
};

struct Texture
{
	HRESULT LoadTexture(ID3D11Device* device);
	bool _loaded = false;
	short _activeUsers = 0;
	std::wstring _filename;
	ID3D11ShaderResourceView* _data = nullptr;
};

struct RenderObject
{
	bool _meshLoaded, _toUnload;
	short _skeleton = -1;
	float _diffuse[4], _specular[4];
	Texture* _diffuseTexture = nullptr;
	Texture* _specularTexture = nullptr;
	std::vector<Mesh> _meshes;
	~RenderObject() { _meshes.clear(); }
};