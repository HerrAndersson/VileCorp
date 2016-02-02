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

	_meshFormatVersion[24] = &AssetManager::ScanModel24;
	_meshFormatVersion[26] = &AssetManager::ScanModel26;
	_meshFormatVersion[27] = &AssetManager::ScanModel27;

	SetupTilesets();

	GetFilenamesInDirectory("Assets/Levels/", ".lvl", *_levelFileNames);
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
bool AssetManager::SetupRenderObjectList(Tileset* tileset)
{
	_modelFiles->clear();
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (string str : tileset->_objects[i])
		{
			_modelFiles->push_back(str);
			RenderObject* renderObject = ScanModel(str);
			if (renderObject == nullptr)
			{
				return false;
			}
			renderObject->_type = (Type)i;
			_renderObjects->push_back(renderObject);
		}
	}

	_activeTileset = tileset;
	return true;
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
		return true; //Specified tileset is already the current tileset
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
			//Tileset was found
			return SetupRenderObjectList(&set);
			//If false one or more .bins in tileset was invalid
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
		renderObject->_mesh._vertexBuffer->Release();
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

bool AssetManager::ParseLevel(int index, vector<GameObjectData> &gameObjects, int &dimX, int &dimY)
{
	LevelHeader lvlHead;

	_infile->open(_levelFileNames->at(index).c_str(), ifstream::binary);
	if (!_infile->is_open())
	{
		return false;
	}

	_infile->read((char*)&lvlHead, sizeof(LevelHeader));
	dimX = lvlHead._tileGridSizeX;
	dimY = lvlHead._tileGridSizeY;

	gameObjects.resize(lvlHead._nrOfGameObjects);
	_infile->read((char*)gameObjects.data(), sizeof(GameObjectData) * lvlHead._nrOfGameObjects);

	_infile->close();
	return true;
}

//Unloads all Assets waiting to be unloaded
void AssetManager::Flush()
{
	for (RenderObject* renderObject : *_renderObjectsToFlush)
	{
		if (renderObject->_toUnload)
		{
			renderObject->_mesh._vertexBuffer->Release();
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
	return (!_activeUsers);
}

//Loads a model to the GPU
bool AssetManager::LoadModel(string fileName, RenderObject* renderObject) {

	string file_path = "Assets/Models/";

	file_path.append(fileName);
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		return false;
	}

	vector<Vertex> vertices;
	vector<WeightedVertex> weightedVertices;

	_infile->seekg(renderObject->_mesh._toMesh);
	if(renderObject->_isSkinned)
	{
		weightedVertices.resize(renderObject->_mesh._vertexBufferSize);
		_infile->read((char*)weightedVertices.data(), renderObject->_mesh._vertexBufferSize*sizeof(WeightedVertex));
	}
	else
	{
		vertices.resize(renderObject->_mesh._vertexBufferSize);
		_infile->read((char*)vertices.data(), renderObject->_mesh._vertexBufferSize*sizeof(Vertex));
	}
	renderObject->_mesh._vertexBuffer = CreateVertexBuffer(&weightedVertices, &vertices, renderObject->_isSkinned);

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
	return true;
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
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		throw std::runtime_error("Failed to open " + file_path);
	}

	int version;
	_infile->read((char*)&version, 4);

	if (version != 24 && version != 26 && version != 27)
	{
		throw std::runtime_error("Failed to load " + file_path + ":\nIncorrect fileversion");
	}
	RenderObject* renderObject = (this->*(_meshFormatVersion[version]))();

	renderObject->_name = fileName;

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

RenderObject* AssetManager::ScanModel27()
{
	RenderObject* renderObject = new RenderObject;
	int skeletonStringLength;
	_infile->read((char*)&skeletonStringLength, 4);
	renderObject->_skeletonName.resize(skeletonStringLength);
	_infile->read((char*)renderObject->_skeletonName.data(), skeletonStringLength);

	renderObject->_isSkinned = strcmp(renderObject->_skeletonName.data(), "Unrigged") != 0;

	_infile->read((char*)&renderObject->_mesh._toMesh, 4);

	MeshHeader26 meshHeader;
	_infile->read((char*)&meshHeader, sizeof(MeshHeader26));

	if (renderObject->_isSkinned)
	{
		_infile->seekg(meshHeader._numberOfVertices * sizeof(WeightedVertex), ios::cur);
	}
	else
	{
		_infile->seekg(meshHeader._numberOfVertices * sizeof(Vertex), ios::cur);
	}

	renderObject->_mesh._pointLights.resize(meshHeader._numberPointLights);
	_infile->read((char*)renderObject->_mesh._pointLights.data(), sizeof(PointlightData) * meshHeader._numberPointLights);

	renderObject->_mesh._spotLights.resize(meshHeader._numberSpotLights);
	_infile->read((char*)renderObject->_mesh._spotLights.data(), sizeof(SpotlightData) * meshHeader._numberSpotLights);

	renderObject->_mesh._vertexBufferSize = meshHeader._numberOfVertices;
	return renderObject;
}

RenderObject* AssetManager::ScanModel26()
{
	RenderObject* renderObject = new RenderObject;
	int skeletonStringLength;
	_infile->read((char*)&skeletonStringLength, 4);
	renderObject->_skeletonName.resize(skeletonStringLength);
	_infile->read((char*)renderObject->_skeletonName.data(), skeletonStringLength);

	renderObject->_isSkinned = strcmp(renderObject->_skeletonName.data(), "Unrigged") != 0;

	_infile->read((char*)&renderObject->_mesh._toMesh, 4);

	MeshHeader26 meshHeader;
	_infile->read((char*)&meshHeader, sizeof(MeshHeader26));

	if (renderObject->_isSkinned)
	{
		_infile->seekg(meshHeader._numberOfVertices*sizeof(WeightedVertex), ios::cur);
	}
	else
	{
		_infile->seekg(meshHeader._numberOfVertices*sizeof(Vertex), ios::cur);
	}

	renderObject->_mesh._pointLights.resize(meshHeader._numberPointLights);
	for (int i = 0; i < meshHeader._numberPointLights; i++)
	{
		_infile->read((char*)&renderObject->_mesh._pointLights[i]._bone, 1);
		_infile->read((char*)&renderObject->_mesh._pointLights[i]._pos, 12);
		_infile->read((char*)&renderObject->_mesh._pointLights[i]._col, 12);
		_infile->read((char*)&renderObject->_mesh._pointLights[i]._intensity, 4);
		renderObject->_mesh._pointLights[i]._range = 100;
	}

	renderObject->_mesh._spotLights.resize(meshHeader._numberSpotLights);
	for (int i = 0; i < meshHeader._numberSpotLights; i++)
	{
		_infile->read((char*)&renderObject->_mesh._spotLights[i]._bone, 1);
		_infile->read((char*)&renderObject->_mesh._spotLights[i]._pos, 12);
		_infile->read((char*)&renderObject->_mesh._spotLights[i]._color, 12);
		_infile->read((char*)&renderObject->_mesh._spotLights[i]._intensity, 4);
		_infile->read((char*)&renderObject->_mesh._spotLights[i]._angle, 4);
		_infile->read((char*)&renderObject->_mesh._spotLights[i]._direction, 12);
		renderObject->_mesh._spotLights[i]._range = 100;
	}
	renderObject->_mesh._vertexBufferSize = meshHeader._numberOfVertices;
	return renderObject;
}

RenderObject* AssetManager::ScanModel24()
{
	RenderObject* renderObject = new RenderObject;
	int meshes;
	_infile->read((char*)&meshes, 4);
	meshes++;
	int skeletonStringLength;
	_infile->read((char*)&skeletonStringLength, 4);
	renderObject->_skeletonName.resize(skeletonStringLength);
	_infile->read((char*)renderObject->_skeletonName.data(), skeletonStringLength);

	renderObject->_isSkinned = strcmp(renderObject->_skeletonName.data(), "Unrigged") != 0;
	_infile->read((char*)&renderObject->_mesh._toMesh, 4);

	for (int i = 0; i < meshes; i++)
	{
		if (i == 0)
		{
			MeshHeader24 meshHeader;
			_infile->read((char*)&meshHeader, sizeof(MeshHeader24));

			_infile->seekg(meshHeader._nameLength, ios::cur);
			if (renderObject->_isSkinned)
			{
				_infile->seekg(meshHeader._numberOfVertices*sizeof(WeightedVertex), ios::cur);
			}
			else
			{
				_infile->seekg(meshHeader._numberOfVertices*sizeof(Vertex), ios::cur);
			}

			renderObject->_mesh._pointLights.resize(meshHeader._numberPointLights);
			for (int i = 0; i < meshHeader._numberPointLights; i++)
			{
				_infile->read((char*)&renderObject->_mesh._pointLights[i]._pos, 12);
				_infile->read((char*)&renderObject->_mesh._pointLights[i]._col, 12);
				_infile->read((char*)&renderObject->_mesh._pointLights[i]._intensity, 4);
				renderObject->_mesh._pointLights[i]._bone = 0;
				renderObject->_mesh._pointLights[i]._range = 100;
			}

			renderObject->_mesh._spotLights.resize(meshHeader._numberSpotLights);
			for (int i = 0; i < meshHeader._numberSpotLights; i++)
			{
				_infile->read((char*)&renderObject->_mesh._spotLights[i]._pos, 12);
				_infile->read((char*)&renderObject->_mesh._spotLights[i]._color, 12);
				_infile->read((char*)&renderObject->_mesh._spotLights[i]._intensity, 4);
				_infile->read((char*)&renderObject->_mesh._spotLights[i]._angle, 4);
				_infile->read((char*)&renderObject->_mesh._spotLights[i]._direction, 12);
				renderObject->_mesh._spotLights[i]._bone = 0;
				renderObject->_mesh._spotLights[i]._range = 100;
			}
			renderObject->_mesh._vertexBufferSize = meshHeader._numberOfVertices;
		}
		else
		{
			MeshHeader24 meshHeader;
			_infile->read((char*)&meshHeader, sizeof(MeshHeader24));

			_infile->seekg(meshHeader._nameLength, ios::cur);
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
				_infile->seekg(sizeof(PointlightData)*meshHeader._numberPointLights, ios::cur);
			}

			if (meshHeader._numberSpotLights)
			{
				_infile->seekg(sizeof(SpotlightData)*meshHeader._numberSpotLights, ios::cur);
			}
		}
	}
	return renderObject;
}

ID3D11Buffer* AssetManager::CreateVertexBuffer(vector<WeightedVertex> *weightedVertices, vector<Vertex> *vertices, int skeleton)
{
	D3D11_BUFFER_DESC vbDESC;
	D3D11_SUBRESOURCE_DATA vertexData;
	vbDESC.Usage = D3D11_USAGE_DEFAULT;
	if (skeleton)
	{
		vbDESC.ByteWidth = sizeof(WeightedVertex)* weightedVertices->size();
		vertexData.pSysMem = weightedVertices->data();
	}
	else
	{
		vbDESC.ByteWidth = sizeof(Vertex)* vertices->size();
		vertexData.pSysMem = vertices->data();
	}
	vbDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDESC.CPUAccessFlags = 0;
	vbDESC.MiscFlags = 0;
	vbDESC.StructureByteStride = 0;
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
RenderObject* AssetManager::GetRenderObject(unsigned index, string texture)
{
	RenderObject* renderObject = nullptr;
	if (index >= 0 && index < _renderObjects->size())
	{
		renderObject = _renderObjects->at(index);
		if (!renderObject->_meshLoaded)
		{
			LoadModel(_modelFiles->at(index), renderObject);
		}
		else if (renderObject->_toUnload)
		{
			renderObject->_toUnload = false;
		}
	}
	if (texture != "")
	{
		renderObject->_diffuseTexture->_data = GetTexture(texture);
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

	string file_path = "Assets/Animations/";

	file_path.append(filename);
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		throw runtime_error("Failed to open " + file_path);
	}

	Skeleton* skeleton = new Skeleton;
	_skeletons->push_back(skeleton);

	SkeletonHeader header;
	_infile->read((char*)&header, sizeof(SkeletonHeader));
	if (header._version != _animationFormatVersion)
	{
		throw runtime_error("Failed to load " + file_path + ":\nIncorrect fileversion");
	}
	int frames;
	XMFLOAT3 scale, translation;
	XMFLOAT4 rotation;
	XMFLOAT4X4 matrixin;
	skeleton->_parents.resize(header._boneCount);
	skeleton->_bindposes = (XMMATRIX*)_aligned_malloc(64 * header._boneCount, 16);
	for (unsigned i = 0; i < header._boneCount; i++)
	{
		_infile->read((char*)&skeleton->_parents[i], sizeof(int));
		_infile->read((char*)&matrixin, sizeof(XMFLOAT4X4));
		skeleton->_bindposes[i] = XMLoadFloat4x4(&matrixin);
	}
	skeleton->_actions.resize(header._actionCount);
	for (uint a = 0; a < skeleton->_actions.size(); a++)
	{
		skeleton->_actions[a]._bones.resize(header._boneCount);
		for (uint b = 0; b < skeleton->_actions[a]._bones.size(); b++)
		{
			_infile->read((char*)&frames, 4);
			skeleton->_actions[a]._bones[b]._frameCount = frames;
			skeleton->_actions[a]._bones[b]._frameTime.resize(frames);
			_infile->read((char*)skeleton->_actions[a]._bones[b]._frameTime.data(), frames * sizeof(int));
			skeleton->_actions[a]._bones[b]._frames.resize(frames);
			
			for (int i = 0; i < frames; i++)
			{
				_infile->read((char*)&translation, sizeof(XMFLOAT3));
				_infile->read((char*)&rotation, sizeof(XMFLOAT4));
				_infile->read((char*)&scale, sizeof(XMFLOAT3));
				skeleton->_actions[a]._bones[b]._frames[i]._translation = XMLoadFloat3(&translation);
				skeleton->_actions[a]._bones[b]._frames[i]._rotation = XMLoadFloat4(&rotation);
				skeleton->_actions[a]._bones[b]._frames[i]._scale = XMLoadFloat3(&scale);
			}
		}
	}

	_infile->close();
	return skeleton;
}