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
#include "rapidjson\reader.h"

using namespace std;
using namespace DirectX;

struct Tileset
{
	string _name;
	vector<string> _floors;
	vector<string> _walls;
	vector<string> _loot;
	vector<string> _spawns;
	vector<string> _traps;
	vector<string> _triggers;
	vector<string> _guards;
	vector<string> _enemies;
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
			_cur = &_tileset->_floors;
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
			_cur = &_tileset->_floors;
		}
		else if (!strcmp("walls", str))
		{
			_cur = &_tileset->_walls;
		}
		else if (!strcmp("loot", str))
		{
			_cur = &_tileset->_loot;
		}
		else if (!strcmp("spawns", str))
		{
			_cur = &_tileset->_spawns;
		}
		else if (!strcmp("traps", str))
		{
			_cur = &_tileset->_traps;
		}
		else if (!strcmp("triggers", str))
		{
			_cur = &_tileset->_triggers;
		}
		else if (!strcmp("guards", str))
		{
			_cur = &_tileset->_guards;
		}
		else if (!strcmp("enemies", str))
		{
			_cur = &_tileset->_enemies;
		}
		return true;
	}
	bool EndObject(rapidjson::SizeType memberCount) { return true; }
	bool StartArray() { return true; }
	bool EndArray(rapidjson::SizeType elementCount) { return true; }
};

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

//AssetManager keeps lists of all assets in the game(currently meshes, textures and skeletal animation), and loads them onto the GPU or memory when required.
//RenderObjects are the interface by which the assetmanager provides models to the renderer. They contain at least one mesh, two colors for diffuse and specular respectively, two handles to textures, a pointer to its skeleton and various help variables.
//Get buffers with:
//assetManager->GetRenderObject(MY_OBJECT_RENDEROBJECT_INDEX)->meshes[SUBMESH_INDEX].vertexBuffer; Also on meshlevel lies the size of the buffer and any lights that may be present.
//Call assetManager->UnLoad(MY_OBJECT_RENDEROBJECT_INDEX, false) if you think the mesh probably won't be needed again soon
//Unless otherwise signed all comments are by Fredrik
class ASSET_MANAGER_EXPORT AssetManager
{
private:
	int _meshFormatVersion = 24;
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

	void LoadModel(string file_path, RenderObject* renderObject);
	void Flush();
	RenderObject* ScanModel(string file_path);
	Texture* ScanTexture(string filename);
	Skeleton* LoadSkeleton(string filename);
	ID3D11Buffer* CreateVertexBuffer(vector<WeightedVertex> *weightedVertices, vector<Vertex> *vertices, int skeleton);
	void SetupRenderObjectList(Tileset* tileset);
	void SetupLevelFileNameList();
	void SetupTilesets();
public:
	AssetManager(ID3D11Device* device);
	~AssetManager();
	RenderObject* GetRenderObject(int index);
	RenderObject* GetRenderObjectByType(Type type, uint index);
	void UnloadModel(int index, bool force);
	void ParseLevel(int index, vector<GameObjectData> &gameObjects, int &dimX, int &dimY);
	bool ActivateTileset(string name);
	ID3D11ShaderResourceView* GetTexture(string filename);
	
};


#endif