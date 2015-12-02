#include "AssetManager.h"


AssetManager::AssetManager(ID3D11Device* device)
{

	_device = device;
	_infile = new ifstream;
	_modelFiles = new vector<string>;
	_renderObjects = new vector<RenderObject*>;
	_renderObjectsToFlush = new vector<RenderObject*>;
	_textures = new vector<Texture*>;
	_texturesToFlush = new vector<Texture*>;
	_levelFileNames = new vector<string>;
	_tilesets = new vector<Tileset>;

	SetupTilesets();
#ifdef _DEBUG
	ActivateTileset("default");
#endif
	SetupLevelFileNameList();
}

AssetManager::~AssetManager()
{
	for (Texture* texture : *_textures)
		if(texture->_loaded)
			texture->_data->Release();
	_textures->clear();

	for (uint i = 0; i < _renderObjects->size(); i++)
	{
		UnloadModel(i, true);
		delete _renderObjects->at(i);
	}
	_renderObjects->clear();
}

//Looks through the Models folder and creates an empty RenderObject for each entry
void AssetManager::SetupRenderObjectList(Tileset* tileset)
{

//#ifdef _DEBUG
//	GetFilenamesInDirectory("../../Output/Bin/x86/Debug/Assets/Models/", ".bin", *_modelFiles);
//#else
//	GetFilenamesInDirectory("Assets/Models/", ".bin", *modelFiles);
//#endif

	for (string str : tileset->floors) _modelFiles->push_back(str);
	for (string str : tileset->walls) _modelFiles->push_back(str);
	for (string str : tileset->deco) _modelFiles->push_back(str);

	for (uint i = 0; i < _modelFiles->size(); i++)
	{
		RenderObject* renderObject = ScanModel(_modelFiles->at(i));
		_renderObjects->push_back(renderObject);
	}
}

void AssetManager::SetupLevelFileNameList()
{
#ifdef _DEBUG
	GetFilenamesInDirectory("../../Output/Bin/x86/Debug/Assets/Levels/", ".lvl", *_levelFileNames);
#else
	GetFilenamesInDirectory("Assets/Levels/", ".lvl", *_levelFileNames);
#endif
}

void AssetManager::SetupTilesets()
{
	vector<string> tilesetFileNames;
#ifdef _DEBUG
	GetFilenamesInDirectory("../../Output/Bin/x86/Debug/Assets/Tilesets/", ".json", tilesetFileNames);
#else
	GetFilenamesInDirectory("Assets/Tilesets/", ".json", tilesetFileNames);
#endif

	TilesetHandler handler;
	handler.tilesets = _tilesets;
	string buffer;
	Reader reader;
	for (string set : tilesetFileNames)
	{
		_infile->open(set, ios::in | ios::ate);
		int size = _infile->tellg();
		_infile->seekg(0);

		buffer.resize(size);
		_infile->read((char*)buffer.data(), size);
		_infile->close();

		StringStream ss(&buffer[0]);
		reader.Parse(ss, handler);
	}
}

void AssetManager::DecrementUsers(Texture* texture)
{
	texture->_activeUsers--;
	if (!texture->_activeUsers)
		_texturesToFlush->push_back(texture);
}

bool AssetManager::ActivateTileset(string name)
{
	for(Tileset set : *_tilesets)
		if (!strcmp(name.c_str(), set.name.c_str()))
		{
			SetupRenderObjectList(&set);
			return true;
		}
	return false;
}

//Marks a model for unloading if running out of memory. Set force to true to unload now
void AssetManager::UnloadModel(int index, bool force)
{
	RenderObject* renderObject = _renderObjects->at(index);
	if (!renderObject->_meshLoaded)
		return;
	if (force)
	{
		for (auto m : renderObject->_meshes)
			m._vertexBuffer->Release();
		renderObject->_toUnload = false;
		if (renderObject->_diffuseTexture != nullptr)
			DecrementUsers(renderObject->_diffuseTexture);
		if (renderObject->_specularTexture != nullptr)
			DecrementUsers(renderObject->_specularTexture);
	}
	else
	{
		_renderObjectsToFlush->push_back(renderObject);
		renderObject->_toUnload = true;
	}
}

void AssetManager::ParseLevel(int index, vector<GameObjectData> &gameObjects, int &dimX, int &dimY)
{
	LevelHeader lvlHead;

	_infile->open(_levelFileNames->at(index).c_str(), ifstream::binary);
	if (!_infile->is_open())
	{
		return;
	}

	_infile->read((char*)&lvlHead, sizeof(LevelHeader));
	dimX = lvlHead._tileGridSizeX;
	dimY = lvlHead._tileGridSizeY;

	gameObjects.resize(lvlHead._nrOfGameObjects);
	_infile->read((char*)gameObjects.data(), sizeof(GameObjectData) * lvlHead._nrOfGameObjects);

	//infile->open(file_path.c_str(), ifstream::binary);
	//if (!infile->is_open())
	//{
	//	return;
	//}

	//infile->seekg(mesh->toMesh);
	//vertices.resize(mesh->vertexBufferSize);
	//infile->read((char*)vertices.data(), mesh->vertexBufferSize*sizeof(Vertex));
}

//Unloads all Assets waiting to be unloaded
void AssetManager::Flush()
{
	for (RenderObject* renderObject : *_renderObjectsToFlush)
		if (renderObject->_toUnload)
		{
			for (Mesh m : renderObject->_meshes)
				m._vertexBuffer->Release();
			renderObject->_toUnload = false;
			if (renderObject->_diffuseTexture != nullptr)
				DecrementUsers(renderObject->_diffuseTexture);
			if (renderObject->_specularTexture != nullptr)
				DecrementUsers(renderObject->_specularTexture);
		}
	_renderObjectsToFlush->clear();

	for (Texture* texture : *_texturesToFlush)
		if (!texture->_activeUsers)
			texture->_data->Release();
	_texturesToFlush->clear();
}

HRESULT Texture::LoadTexture(ID3D11Device* device)
{
	HRESULT res;
	if (!_loaded)
	{
#ifdef _DEBUG
		_filename.insert(0, L"../../Output/Bin/x86/Debug/Assets/Textures/");
		res = DirectX::CreateWICTextureFromFile(device, _filename.c_str(), nullptr, &_data, 0);
#else
		_filename.insert(0, L"Assets/Textures/");
		res = DirectX::CreateWICTextureFromFile(device, _filename.c_str(), nullptr, &_data, 0);
#endif
		_loaded = true;
		_activeUsers++;
	}
	return res;
}

//Loads a model to the GPU
void AssetManager::LoadModel(string file_path, RenderObject* renderObject) {
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		return;
	}

	Mesh* mesh;
	vector<Vertex> vertices;

	for (uint i = 0; i < renderObject->_meshes.size(); i++)
	{
		mesh = &renderObject->_meshes[i];
		_infile->seekg(mesh->_toMesh);
		vertices.resize(mesh->_vertexBufferSize);
		_infile->read((char*)vertices.data(), mesh->_vertexBufferSize*sizeof(Vertex));
		mesh->_vertexBuffer = CreateVertexBuffer(&vertices, renderObject->_skeleton);
	}

	if (renderObject->_diffuseTexture != nullptr)
		if (renderObject->_diffuseTexture->LoadTexture(_device) == E_OUTOFMEMORY)
		{
			Flush();
			renderObject->_diffuseTexture->LoadTexture(_device);//TODO handle if still out of memory - Fredrik
		}
	if (renderObject->_specularTexture != nullptr)
		if(renderObject->_specularTexture->LoadTexture(_device) == E_OUTOFMEMORY)
		{
			Flush();
			renderObject->_specularTexture->LoadTexture(_device);//TODO handle if still out of memory - Fredrik
		}

	_infile->close();
}

Texture* AssetManager::ScanTexture(string filename)
{
	for (Texture* texture : *_textures)
	{
		string str = string(texture->_filename.begin(), texture->_filename.end());
		if (!strcmp(str.data(), filename.data()))
			return texture;
	}
	Texture* texture = new Texture;
	texture->_filename = wstring(filename.begin(), filename.end());
	_textures->push_back(texture);
	return texture;
}

//Creates a RenderObject for the specified model without loading it
RenderObject* AssetManager::ScanModel(string file_path)
{
	RenderObject* renderObject = new RenderObject;
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		return renderObject;
	}

	MainHeader mainHeader;
	_infile->read((char*)&mainHeader, sizeof(MainHeader));
	mainHeader._meshCount++;
	if (mainHeader._version != _meshFormatVersion)
		throw std::runtime_error("Failed to load " + file_path + ":\nIncorrect fileversion");
	for (int i = 0; i < mainHeader._meshCount; i++)
	{
		Mesh mesh;
		_infile->read((char*)&mesh._toMesh, 4);
		renderObject->_meshes.push_back(mesh);
	}

	for (int i = 0; i < mainHeader._meshCount; i++)
	{
		MeshHeader meshHeader;
		_infile->read((char*)&meshHeader, sizeof(MeshHeader));

		renderObject->_meshes[i]._name.resize(meshHeader._nameLength);

		_infile->read((char*)renderObject->_meshes[i]._name.data(), meshHeader._nameLength);
		_infile->seekg(meshHeader._numberOfVertices*sizeof(Vertex), ios::cur);

		if (meshHeader._numberPointLights)
		{
			renderObject->_meshes[i]._pointLights.resize(sizeof(PointLight)*meshHeader._numberPointLights);
			_infile->read((char*)renderObject->_meshes[i]._pointLights.data(), sizeof(PointLight)*meshHeader._numberPointLights);
		}

		if (meshHeader._numberSpotLights)
		{
			renderObject->_meshes[i]._spotLights.resize(sizeof(SpotLight)*meshHeader._numberSpotLights);
			_infile->read((char*)renderObject->_meshes[i]._spotLights.data(), sizeof(SpotLight)*meshHeader._numberSpotLights);
		}
		renderObject->_meshes[i]._vertexBufferSize = meshHeader._numberOfVertices;
	}
	MatHeader matHeader;
	_infile->read((char*)&matHeader, sizeof(MatHeader));
	_infile->read((char*)&renderObject->_diffuse, 16);
	_infile->read((char*)&renderObject->_specular, 16);

	string diffFile, specFile;
	diffFile.resize(matHeader._diffuseNameLength);
	specFile.resize(matHeader._specularNameLength);
	_infile->read((char*)diffFile.data(), matHeader._diffuseNameLength);
	_infile->read((char*)specFile.data(), matHeader._specularNameLength);

	if (matHeader._diffuseNameLength)
		renderObject->_diffuseTexture = ScanTexture(diffFile);
	if (matHeader._specularNameLength)
		renderObject->_specularTexture = ScanTexture(specFile);

	_infile->close();

	renderObject->_meshLoaded = false;

	return renderObject;
}

ID3D11Buffer* AssetManager::CreateVertexBuffer(vector<Vertex> *vertices, int skeleton)
{
	D3D11_BUFFER_DESC vbDESC;
	vbDESC.Usage = D3D11_USAGE_DEFAULT;
	vbDESC.ByteWidth = sizeof(Vertex)* vertices->size();
	vbDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDESC.CPUAccessFlags = 0;
	vbDESC.MiscFlags = 0;
	vbDESC.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;

	vertexData.pSysMem = vertices->data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Buffer* vertexBuffer;

	HRESULT result = _device->CreateBuffer(&vbDESC, &vertexData, &vertexBuffer);
	if (result == E_OUTOFMEMORY)
	{
		Flush();
		result = _device->CreateBuffer(&vbDESC, &vertexData, &vertexBuffer);//TODO handle if still out of memory - Fredrik
	}

	return vertexBuffer;
}

//How AssetManager interfaces with the renderer. Don't save the return, request it anew everytime unless you are certain the model won't be unloaded
//currently two objects exist. 0 = man 1 = cube, 2 = cube with cubes
RenderObject* AssetManager::GetRenderObject(int index)
{
	RenderObject* renderObject = _renderObjects->at(index);
	if (!renderObject->_meshLoaded)
		LoadModel(_modelFiles->at(index), renderObject);
	else if (renderObject->_toUnload)
		renderObject->_toUnload = false;
	return renderObject;
}
