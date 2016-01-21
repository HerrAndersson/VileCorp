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
	_skeletons = new vector<Skeleton*>;

	SetupTilesets();

	SetupLevelFileNameList();
}

AssetManager::~AssetManager()
{
	for (Texture* texture : *_textures)
	{
		if (texture->_loaded)
		{
			texture->_data->Release();
		}
		delete texture;
	}
	for (uint i = 0; i < _renderObjects->size(); i++)
	{
		delete _renderObjects->at(i);
	}
	for (uint i = 0; i < _skeletons->size(); i++)
	{
		delete _skeletons->at(i);
	}
	delete _infile;
	delete _modelFiles;
	delete _renderObjects;
	delete _renderObjectsToFlush;
	delete _textures;
	delete _texturesToFlush;
	delete _levelFileNames;
	delete _tilesets;
	delete _skeletons;
}

//Looks through the Models folder and creates an empty RenderObject for each entry
void AssetManager::SetupRenderObjectList(Tileset* tileset)
{
	_modelFiles->clear();
	vector<string>* vec = &tileset->_floors;
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (string str : *vec)
		{
			_modelFiles->push_back(str);
			RenderObject* renderObject = ScanModel(str);
			renderObject->_type = (Type)i;
			_renderObjects->push_back(renderObject);
		}
		vec++;
	}

	_activeTileset = tileset;
}

void AssetManager::SetupLevelFileNameList()
{
	GetFilenamesInDirectory("Assets/Levels/", ".lvl", *_levelFileNames);
}

void AssetManager::SetupTilesets()
{
	vector<string> tilesetFileNames;

	GetFilenamesInDirectory("Assets/Tilesets/", ".json", tilesetFileNames);

	TilesetHandler handler;
	handler._tilesets = _tilesets;
	string buffer;
	rapidjson::Reader reader;
	for (string set : tilesetFileNames)
	{
		_infile->open(set, ios::in | ios::ate);
		int size = _infile->tellg();
		_infile->seekg(0);

		buffer.resize(size);
		_infile->read((char*)buffer.data(), size);
		_infile->close();

		rapidjson::StringStream ss(&buffer[0]);
		reader.Parse(ss, handler);
	}
	SetupRenderObjectList(&_tilesets->at(0));
}

//Do not call, call _objectHandler->ActivateTileset() - Fredrik
bool AssetManager::ActivateTileset(string name)
{
	if (!strcmp(name.c_str(), _activeTileset->_name.c_str()))
	{
		return true;
	}

	for (Texture* texture : *_textures)
	{
		if (texture->_loaded)
		{
			texture->_data->Release();
		}
		delete texture;
	}
	for (uint i = 0; i < _renderObjects->size(); i++)
	{
		delete _renderObjects->at(i);
	}
	for (uint i = 0; i < _skeletons->size(); i++)
	{
		delete _skeletons->at(i);
	}
	_textures->clear();
	_renderObjects->clear();
	_skeletons->clear();

	for (Tileset set : *_tilesets)
	{
		if (!strcmp(name.c_str(), set._name.c_str()))
		{
			SetupRenderObjectList(&set);
			return true;
		}
	}
	return false;
}

//Marks a model for unloading if running out of memory. Set force to true to unload now
void AssetManager::UnloadModel(int index, bool force)
{
	RenderObject* renderObject = _renderObjects->at(index);
	if (!renderObject->_meshLoaded)
	{
		return;
	}
	if (force)
	{
		for (auto m : renderObject->_meshes)
		{
			m._vertexBuffer->Release();
		}
		renderObject->_toUnload = false;
		renderObject->_meshLoaded = false;
		if (renderObject->_diffuseTexture != nullptr)
		{
			if (renderObject->_diffuseTexture->DecrementUsers())
			{
				_texturesToFlush->push_back(renderObject->_diffuseTexture);
			}
		}
		if (renderObject->_specularTexture != nullptr)
		{
			if (renderObject->_specularTexture->DecrementUsers())
			{
				_texturesToFlush->push_back(renderObject->_specularTexture);
			}
		}
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

	_infile->close();
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
	{
		if (renderObject->_toUnload)
		{
			for (Mesh m : renderObject->_meshes)
			{
				m._vertexBuffer->Release();
			}
			renderObject->_toUnload = false;
			renderObject->_meshLoaded = false;
			if (renderObject->_diffuseTexture != nullptr)
			{
				if (renderObject->_diffuseTexture->DecrementUsers())
				{
					_texturesToFlush->push_back(renderObject->_diffuseTexture);
				}
			}
			if (renderObject->_specularTexture != nullptr)
			{
				if (renderObject->_specularTexture->DecrementUsers())
				{
					_texturesToFlush->push_back(renderObject->_specularTexture);
				}
			}
		}
	}
	_renderObjectsToFlush->clear();

	for (Texture* texture : *_texturesToFlush)
	{
		if (!texture->_activeUsers)
		{
			texture->_data->Release();
		}
	}
	_texturesToFlush->clear();
}

HRESULT Texture::LoadTexture(ID3D11Device* device)
{
	HRESULT res = S_OK;
	if (!_loaded)
	{
		res = DirectX::CreateWICTextureFromFile(device, _filename.c_str(), nullptr, &_data, 0);

		if (_data == nullptr)
		{
			string filenameString(_filename.begin(),_filename.end());
			throw std::runtime_error("Texture " + filenameString + " not found");
		}
		_loaded = true;
	}
	_activeUsers++;
	return res;
}

bool Texture::DecrementUsers()
{
	_activeUsers--;
	if (!_activeUsers)
	{
		return true;
	}
	return false;
}

//Loads a model to the GPU
void AssetManager::LoadModel(string fileName, RenderObject* renderObject) {

	string file_path = "Assets/Models/";

	file_path.append(fileName);
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		return;
	}

	Mesh* mesh;
	vector<Vertex> vertices;
	vector<WeightedVertex> weightedVertices;

	for (uint i = 0; i < renderObject->_meshes.size(); i++)
	{
		mesh = &renderObject->_meshes[i];
		_infile->seekg(mesh->_toMesh);
		if(renderObject->_isSkinned)
		{
			weightedVertices.resize(mesh->_vertexBufferSize);
			_infile->read((char*)weightedVertices.data(), mesh->_vertexBufferSize*sizeof(WeightedVertex));
		}
		else
		{
			vertices.resize(mesh->_vertexBufferSize);
			_infile->read((char*)vertices.data(), mesh->_vertexBufferSize*sizeof(Vertex));
		}
		mesh->_vertexBuffer = CreateVertexBuffer(&weightedVertices, &vertices, renderObject->_isSkinned);
	}

	if (renderObject->_diffuseTexture != nullptr)
	{
		if (renderObject->_diffuseTexture->LoadTexture(_device) == E_OUTOFMEMORY)
		{
			Flush();
			renderObject->_diffuseTexture->LoadTexture(_device);//TODO handle if still out of memory - Fredrik
		}
	}
	if (renderObject->_specularTexture != nullptr)
	{
		if (renderObject->_specularTexture->LoadTexture(_device) == E_OUTOFMEMORY)
		{
			Flush();
			renderObject->_specularTexture->LoadTexture(_device);//TODO handle if still out of memory - Fredrik
		}
	}

	_infile->close();
	renderObject->_meshLoaded = true;
}

Texture* AssetManager::ScanTexture(string filename)
{
	for (Texture* texture : *_textures)
	{
		string str = string(texture->_filename.begin(), texture->_filename.end());
		if (!strcmp(str.data(), filename.data()))
		{
			return texture;
		}
	}
	Texture* texture = new Texture;
	texture->_filename = wstring(filename.begin(), filename.end());

	texture->_filename.insert(0, L"Assets/Textures/");

	_textures->push_back(texture);
	return texture;
}

//Creates a RenderObject for the specified model without loading it
RenderObject* AssetManager::ScanModel(string fileName)
{

	string file_path = "Assets/Models/";

	file_path.append(fileName);
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
	{
		throw std::runtime_error("Failed to load " + file_path + ":\nIncorrect fileversion");
	}
	int skeletonStringLength;
	_infile->read((char*)&skeletonStringLength, 4);
	renderObject->_skeletonName.resize(skeletonStringLength);
	_infile->read((char*)renderObject->_skeletonName.data(), skeletonStringLength);

	renderObject->_isSkinned = strcmp(renderObject->_skeletonName.data(), "Unrigged") != 0;

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
		if (renderObject->_isSkinned)
		{
			_infile->seekg(meshHeader._numberOfVertices*sizeof(WeightedVertex), ios::cur);
		}
		else
		{
			_infile->seekg(meshHeader._numberOfVertices*sizeof(Vertex), ios::cur);
		}

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
	{
		renderObject->_diffuseTexture = ScanTexture(diffFile);
	}
	if (matHeader._specularNameLength)
	{
		renderObject->_specularTexture = ScanTexture(specFile);
	}

	_infile->close();

	if (renderObject->_isSkinned)
	{
		renderObject->_skeleton = LoadSkeleton(renderObject->_skeletonName);
	}

	renderObject->_meshLoaded = false;

	return renderObject;
}

ID3D11Buffer* AssetManager::CreateVertexBuffer(vector<WeightedVertex> *weightedVertices, vector<Vertex> *vertices, int skeleton)
{
	D3D11_BUFFER_DESC vbDESC;
	vbDESC.Usage = D3D11_USAGE_DEFAULT;
	if (skeleton)
	{
		vbDESC.ByteWidth = sizeof(WeightedVertex)* weightedVertices->size();
	}
	else
	{
		vbDESC.ByteWidth = sizeof(Vertex)* vertices->size();
	}
	vbDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDESC.CPUAccessFlags = 0;
	vbDESC.MiscFlags = 0;
	vbDESC.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	if (skeleton)
	{
		vertexData.pSysMem = weightedVertices->data();
	}
	else
	{
		vertexData.pSysMem = vertices->data();
	}
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
RenderObject* AssetManager::GetRenderObject(int index)
{
	RenderObject* renderObject = _renderObjects->at(index);
	if (!renderObject->_meshLoaded)
	{
		LoadModel(_modelFiles->at(index), renderObject);
	}
	else if (renderObject->_toUnload)
	{
		renderObject->_toUnload = false;
	}
	return renderObject;
}

uint AssetManager::GetRenderObjectByType(Type type, uint index)
{
	uint i = 0;
	uint returnIndex = 0;
	for (RenderObject* renderObject : *_renderObjects)
	{
		if (renderObject->_type == type)
		{
			if (i == index)
			{
				return returnIndex;
			}
			i++;
		}
		i = 0;
		returnIndex++;
	}
	return 0;
}

ID3D11ShaderResourceView* AssetManager::GetTexture(string filename)
{
	Texture* texture = ScanTexture(filename);
	texture->LoadTexture(_device);
	return texture->_data;
}

Skeleton* AssetManager::LoadSkeleton(string filename)
{

	for (Skeleton* skeleton : *_skeletons)
	{
		if (strcmp(skeleton->_name.c_str(), filename.data()))
		{
			return skeleton;
		}
	}

	string file_path = "Assets/Models/";

	file_path.append(filename);
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		throw runtime_error("Failed to open " + file_path);
	}

	_skeletons->push_back(new Skeleton);
	Skeleton* skeleton = _skeletons->back();

	SkeletonHeader header;
	_infile->read((char*)&header, sizeof(SkeletonHeader));
	if (header._version != _animationFormatVersion)
	{
		throw runtime_error("Failed to load " + file_path + ":\nIncorrect fileversion");
	}

	vector<int> frameCountPerAction;
	frameCountPerAction.resize(header._actionCount);
	_infile->read((char*)frameCountPerAction.data(), header._actionCount * 4);

	skeleton->_skeleton.resize(header._boneCount);
	_infile->read((char*)skeleton->_skeleton.data(), header._boneCount * sizeof(Bone));

	skeleton->_actions.resize(header._actionCount);
	for (uint a = 0; a < skeleton->_actions.size(); a++)
	{
		skeleton->_actions[a]._frameTime.resize(frameCountPerAction[a]);
		_infile->read((char*)skeleton->_actions[a]._frameTime.data(), frameCountPerAction[a] * 4);

		skeleton->_actions[a]._bones.resize(header._boneCount);
		for (uint b = 0; b < skeleton->_actions[a]._bones.size(); b++)
		{
			skeleton->_actions[a]._bones[b]._frames.resize(frameCountPerAction[a]);
			_infile->read((char*)skeleton->_actions[a]._bones[b]._frames.data(), sizeof(Frame) * frameCountPerAction[a]);
		}
	}

	_infile->close();
	return skeleton;
}