#ifndef ASSET_MANAGER
#define ASSET_MANAGER
#define ASSET_MANAGER_EXPORT __declspec(dllexport)
#define uint unsigned int

#include <d3d11.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <fstream>
#include "WICTextureLoader.h"
#include "RenderUtils.h"

using namespace std;
using namespace DirectX;

struct MainHeader 
{
	int _version, _meshCount;
};

struct MeshHeader
{
	int _nameLength, _numberOfVertices, _subMeshID, _numberPointLights, _numberSpotLights;
};

struct MatHeader 
{
	int _diffuseNameLength, _specularNameLength;
};

struct LevelHeader
{
	int _version, _tileGridSizeX, _tileGridSizeY, _nrOfGameObjects;
};

struct GameObjectData
{
	int _posX, _posZ;
	float _rotY;
	int _tileType;
	bool _walkable;
};

static void splitStringToVector(string input, vector<string> &output, string delimiter) {
	size_t pos = 0;
	string token;
	while ((pos = input.find(delimiter)) != string::npos)
	{
		token = input.substr(0, pos);
		if (!token.empty())
		{
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
	if (hFind != INVALID_HANDLE_VALUE)
	{
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
	int _meshFormatVersion = 23;
	ifstream* _infile;
	ID3D11Device* _device;

	vector<string>* _modelFiles;
	vector<string>* _levelFileNames;

	vector<RenderObject*>* _renderObjects;
	vector<RenderObject*>* _renderObjectsToFlush;

	vector<Texture*>* _textures;
	vector<Texture*>* _texturesToFlush;

	void LoadModel(string file_path, RenderObject* renderObject);
	void Flush();
	RenderObject* ScanModel(string file_path);
	Texture* ScanTexture(string filename);
	void DecrementUsers(Texture* texture);
	ID3D11Buffer* CreateVertexBuffer(vector<Vertex> *vertices, int skeleton);
	void SetupRenderObjectList();
	void SetupLevelFileNameList();

public:
	AssetManager(ID3D11Device* device);
	~AssetManager();
	RenderObject* GetRenderObject(int index);
	void UnloadModel(int index, bool force);
	void ParseLevel(int index, vector<GameObjectData> &gameObjects, int &dimX, int &dimY);
	
};


#endif