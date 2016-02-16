#pragma once
#define ASSET_MANAGER_EXPORT __declspec(dllexport)
#define uint unsigned int

#include <d3d11.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <fstream>
#include "WICTextureLoader.h"
#include "RenderUtils.h"
#include "LevelFormat.h"
#include "cereal\cereal.hpp"
#include "cereal\archives\json.hpp"
#include "cereal\types\vector.hpp"
#include "cereal\types\string.hpp"

using namespace std;
using namespace DirectX;

//Disable warning about std::Map dll-interface -> typedef std::map<int, AssetManager::_scanFunc> _scanFuncMap;
#pragma warning( disable: 4251 )

struct MeshHeader24
{
	int _nameLength, _numberOfVertices, _subMeshID, _numberPointLights, _numberSpotLights;
};

struct MeshHeader26
{
	int _numberOfVertices, _numberPointLights, _numberSpotLights;
};

struct MatHeader 
{
	int _diffuseNameLength, _specularNameLength;
};

struct SkeletonHeader
{
	unsigned int _version, _framerate, _boneCount, _actionCount;
};

static bool GetFilenamesInDirectory(char* folder, char* extension, vector<string> &listToFill)
{
	bool result = false;
	if (folder != nullptr && extension != nullptr)
	{
		ofstream testout;
		string search_path = folder;
		search_path.append("*");
		search_path.append(extension);
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			result = true;
			do {
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					string path = fd.cFileName;
					listToFill.push_back(folder + path);
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
	}
	return result;
}

//AssetManager keeps lists of all assets in the game(currently meshes, textures and skeletal animation), and loads them onto the GPU or memory when required.
//RenderObjects are the interface by which the assetmanager provides models to the renderer. They contain at least one mesh, two colors for diffuse and specular respectively, two handles to textures, a pointer to its skeleton and various help variables.
//Get buffers with:
//assetManager->GetRenderObject(MY_OBJECT_RENDEROBJECT_INDEX).vertexBuffer; Also on meshlevel lies the size of the buffer and any lights that may be present.
//Call assetManager->UnLoad(MY_OBJECT_RENDEROBJECT_INDEX, false) if you think the mesh probably won't be needed again soon
//Unless otherwise signed all comments are by Fredrik
class ASSET_MANAGER_EXPORT AssetManager
{
private:

	typedef Mesh* (AssetManager::*_scanFunc)();
	typedef std::map<int, AssetManager::_scanFunc> _scanFuncMap;

	_scanFuncMap _meshFormatVersion;
	int _animationFormatVersion = 10;
	ifstream* _infile;
	ID3D11Device* _device;
	vector<string>* _levelFileNames;
	vector<Skeleton*>* _skeletons;

	vector<RenderObject*>* _renderObjects;

	vector<Texture*>* _textures;
	vector<Mesh*>* _meshes;

	LevelFormat _currentLevelData;

	bool LoadModel(string name, Mesh* mesh);
	void Flush();
	Mesh* ScanModel24();
	Mesh* ScanModel26();
	Mesh* ScanModel27();
	Mesh* ScanModel(string name);
	Texture* ScanTexture(string name);
	Mesh* GetModel(string name);
	Skeleton* LoadSkeleton(string name);
	ID3D11Buffer* CreateVertexBuffer(vector<WeightedVertex> *weightedVertices, vector<Vertex> *vertices, int skeleton);
public:
	AssetManager(ID3D11Device* device);
	~AssetManager();
	RenderObject* GetRenderObject(int index);
	RenderObject* GetRenderObject(string meshName, string textureName);
	LevelFormat* ParseLevel(int index);
	Texture* GetTexture(string name);
};
