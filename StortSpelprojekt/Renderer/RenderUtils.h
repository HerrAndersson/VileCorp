#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <DirectXMath.h>

struct WeightedVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	int boneIndices[4];
	float boneWeights[4];
};

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

struct PointLight
{
	float pos[3], col[3], intensity;
};

struct SpotLight
{
	float pos[3], col[3], intensity, angle, direction[3];
};

struct Mesh
{
	~Mesh() {
		pointLights.clear();
		spotLights.clear();
	}
	std::string name;
	ID3D11Buffer* vertexBuffer;
	int vertexBufferSize, toMesh;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
};

struct Texture
{
	void LoadTexture(ID3D11Device* device);
	bool loaded = false;
	short activeUsers = 0;
	std::wstring filename;
	ID3D11ShaderResourceView* data = nullptr;
};

struct RenderObject
{
	bool meshLoaded, toUnload;
	short skeleton = -1;
	float diffuse[4], specular[4];
	Texture* diffuseTexture = nullptr;
	Texture* specularTexture = nullptr;
	std::vector<Mesh> meshes;
};
