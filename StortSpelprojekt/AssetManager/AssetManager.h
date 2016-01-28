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
#include "rapidjson\reader.h"

using namespace std;
using namespace DirectX;

struct Tileset
{
	Tileset()
	{
		for (int i = 0; i < NR_OF_TYPES; i++)
		{
			vector<string> type;
			_objects.push_back(type);
		}
	}
	string _name;
	vector<vector<string>> _objects;
};

struct TilesetHandler
{
	vector<Tileset>* _tilesets;
	Tileset* _tileset;
	vector<string>* _cur;
	bool _nameNext = false;

	bool Null() { return true; }
	bool Bool(bool b) { return true; }
	bool Int(int i) { return true; }
	bool Uint(unsigned u) { return true; }
	bool Int64(int64_t i) { return true; }
	bool Uint64(uint64_t u) { return true; }
	bool Double(double d) { return true; }
	bool String(const char* str, rapidjson::SizeType length, bool copy)
	{
		if (_nameNext)
		{
			_tileset->_name = str;
			_nameNext = false;
			_cur = &_tileset->_objects[FLOOR];
		}
		else
		{
			_cur->push_back(str);
		}
		return true;
	}
	bool StartObject()
	{
		Tileset newTileset;
		_tilesets->push_back(newTileset);
		_tileset = &_tilesets->back();
		return true;
	}
	bool Key(const char* str, rapidjson::SizeType length, bool copy)
	{
		if (!strcmp("name", str))
		{
			_nameNext = true;
		}
		else if (!strcmp("floors", str))
		{
			_cur = &_tileset->_objects[FLOOR];
		}
		else if (!strcmp("walls", str))
		{
			_cur = &_tileset->_objects[WALL];
		}
		else if (!strcmp("loot", str))
		{
			_cur = &_tileset->_objects[LOOT];
		}
		else if (!strcmp("spawns", str))
		{
			_cur = &_tileset->_objects[SPAWN];
		}
		else if (!strcmp("traps", str))
		{
			_cur = &_tileset->_objects[TRAP];
		}
		else if (!strcmp("triggers", str))
		{
			_cur = &_tileset->_objects[TRIGGER];
		}
		else if (!strcmp("guards", str))
		{
			_cur = &_tileset->_objects[GUARD];
		}
		else if (!strcmp("enemies", str))
		{
			_cur = &_tileset->_objects[ENEMY];
		}
		return true;
	}
	bool EndObject(rapidjson::SizeType memberCount) { return true; }
	bool StartArray() { return true; }
	bool EndArray(rapidjson::SizeType elementCount) { return true; }
};

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

struct LevelHeader
{
	int _version, _tileGridSizeX, _tileGridSizeY, _nrOfGameObjects;
};

struct SkeletonHeader
{
	unsigned int _version, _framerate, _actionCount, _boneCount;
};

struct GameObjectData
{
	int _posX, _posZ;
	float _rotY;
	int _tileType;
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
	typedef RenderObject* (AssetManager::*_scanFunc)();
	typedef std::map<int, AssetManager::_scanFunc> _scanFuncMap;
	_scanFuncMap _meshFormatVersion;
	int _animationFormatVersion = 10;
	ifstream* _infile;
	ID3D11Device* _device;
	Tileset* _activeTileset;

	vector<string>* _modelFiles;
	vector<string>* _levelFileNames;
	vector<Tileset>* _tilesets;
	vector<Skeleton*>* _skeletons;

	vector<RenderObject*>* _renderObjects;
	vector<RenderObject*>* _renderObjectsToFlush;

	vector<Texture*>* _textures;
	vector<Texture*>* _texturesToFlush;

	bool LoadModel(string file_path, RenderObject* renderObject);
	void Flush();
	RenderObject* ScanModel24();
	RenderObject* ScanModel26();
	RenderObject* ScanModel27();
	RenderObject* ScanModel(string file_path);
	Texture* ScanTexture(string filename);
	Skeleton* LoadSkeleton(string filename);
	ID3D11Buffer* CreateVertexBuffer(vector<WeightedVertex> *weightedVertices, vector<Vertex> *vertices, int skeleton);
	bool SetupRenderObjectList(Tileset* tileset);
	void SetupTilesets();
public:
	AssetManager(ID3D11Device* device);
	~AssetManager();
	RenderObject* GetRenderObject(int index);
	uint GetRenderObjectByType(Type type, uint index);
	void UnloadModel(int index, bool force);
	bool ParseLevel(int index, vector<GameObjectData> &gameObjects, int &dimX, int &dimY);
	bool ActivateTileset(string name);
	ID3D11ShaderResourceView* GetTexture(string filename);
};