#ifndef ASSET_MANAGER
#define ASSET_MANAGER
#define ASSET_MANAGER_EXPORT __declspec(dllexport)

#include <d3d11.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <fstream>
#include <Windows.h>

using namespace std;
using namespace DirectX;

struct MainHeader {
	int version, meshCount;
};

struct MeshHeader
{
	int nameLength, numberOfVertices, subMeshID, numberPointLights, numberSpotLights;
};

struct MatHeader {
	int diffuseNameLength, specularNameLength;
};

struct Point
{
	XMFLOAT3 position;
	int boneIndices[4];
	float boneWeights[4];
};

struct WeightedVertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	int boneIndices[4];
	float boneWeights[4];
};

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
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
	string name;
	ID3D11Buffer* vertexBuffer;
	int vertexBufferSize, toMesh;
	vector<PointLight> pointLights;
	vector<SpotLight> spotLights;
};

struct Texture
{
	void LoadTexture();
	bool loaded = false;
	short activeUsers = 0;
	string filename;
	ID3D11ShaderResourceView* data = nullptr;
};

struct RenderObject
{
	bool meshLoaded, toUnload;
	short skeleton = -1;
	float diffuse[4], specular[4];
	Texture* diffuseTexture = nullptr;
	Texture* specularTexture = nullptr;
	vector<Mesh> meshes;
};

static void splitStringToVector(string input, vector<string> &output, string delimiter) {
	size_t pos = 0;
	string token;
	while ((pos = input.find(delimiter)) != string::npos) {
		token = input.substr(0, pos);
		if (!token.empty()) {
			output.push_back(token);
		}
		input.erase(0, pos + delimiter.length());
	}
	output.push_back(input);
};

static bool GetFilenamesInDirectory(char* folder, char* extension, vector<string> &listToFill)
{
	ofstream testout;
	string search_path = folder;
	search_path.append("*");
	search_path.append(extension);
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				string path = fd.cFileName;
				listToFill.push_back(folder + path);
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
	return true;
}

//AssetManager keeps lists of all assets in the game(currently meshes), and loads them onto the GPU or memory when required.
//RenderObjects are the interface by which the assetmanager provides models to the renderer. They contain at least one mesh, two colors for diffuse and specular respectively, two handles to textures(textures not yet in), an index to its skeleton and help variables.
//Get buffers with:
//assetManager->GetRenderObject(MY_OBJECT_RENDEROBJECT_INDEX)->meshes[SUBMESH_INDEX].vertexBuffer; Also on meshlevel lies the size of the buffer and any lights that may be present.
//Call assetManager->UnLoad(MY_OBJECT_RENDEROBJECT_INDEX, false) if you think the mesh probably won't be needed again soon
//Unless otherwise signed all comments are by Fredrik
class ASSET_MANAGER_EXPORT AssetManager
{
private:
	int meshFormatVersion = 23;
	ifstream* infile;
	ID3D11Device* device;

	vector<string>* modelFiles;

	vector<RenderObject*>* renderObjects;
	vector<RenderObject*>* renderObjectsToFlush;

	vector<Texture*>* textures;
	vector<Texture*>* texturesToFlush;

	void LoadModel(string file_path, RenderObject* renderObject);
	void Flush();
	RenderObject* ScanModel(string file_path);
	Texture* ScanTexture(string filename);
	void DecrementUsers(Texture* texture);
	ID3D11Buffer* CreateVertexBuffer(vector<Vertex> *vertices, int skeleton);
	void SetupRenderObjectList();

public:
	AssetManager(ID3D11Device* device);
	~AssetManager();
	RenderObject* GetRenderObject(int index);
	void UnloadModel(int index, bool force);
};


#endif