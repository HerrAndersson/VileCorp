#include "AssetManager.h"


AssetManager::AssetManager(ID3D11Device* device)
{
	_device = device;
	_infile = new ifstream;
	_renderObjects = new vector<RenderObject*>;
	_meshes = new vector<Mesh*>;
	_textures = new vector<Texture*>;
	_levelFileNames = new vector<string>;
	_skeletons = new vector<Skeleton*>;

	_meshFormatVersion[24] = &AssetManager::ScanModel24;
	_meshFormatVersion[26] = &AssetManager::ScanModel26;
	_meshFormatVersion[27] = &AssetManager::ScanModel27;
	//_meshFormatVersion[28] = &AssetManager::ScanModel28;

	GetFilenamesInDirectory((char*)LEVEL_FOLDER_PATH.c_str(), ".lvl", *_levelFileNames);
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
	delete _renderObjects;
	delete _textures;
	delete _meshes;
	delete _levelFileNames;
	delete _skeletons;
}

//Unloads all Assets waiting to be unloaded
void AssetManager::Flush()
{
	for (Mesh* mesh : *_meshes)
	{
		if (!mesh->_activeUsers)
		{
			mesh->_vertexBuffer->Release();
			mesh->_meshLoaded = false;
		}
	}
	_meshes->clear();

	for (Texture* texture : *_textures)
	{
		if (!texture->_activeUsers)
		{
			texture->_data->Release();
		}
	}
	_textures->clear();
}

HRESULT Texture::LoadTexture(ID3D11Device* device)
{
	HRESULT res = S_OK;
	if (!_loaded)
	{
		wstring _filePath = TEXTURE_FOLDER_PATH_W;
		_filePath.append(_name.begin(), _name.end());
		res = DirectX::CreateWICTextureFromFile(device, _filePath.c_str(), nullptr, &_data, 0);

		if (_data == nullptr)
		{
 			string filenameString(_filePath.begin(), _filePath.end());
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

bool Mesh::DecrementUsers()
{
	_activeUsers--;
	return (!_activeUsers);
}

//Loads a model to the GPU
bool AssetManager::LoadModel(string name, Mesh* mesh) {

	string file_path = MODEL_FOLDER_PATH;

	file_path.append(name);
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		return false;
	}

	vector<Vertex> vertices;
	vector<WeightedVertex> weightedVertices;

	_infile->seekg(mesh->_toMesh);
	if(mesh->_isSkinned)
	{
		weightedVertices.resize(mesh->_vertexBufferSize);
		_infile->read((char*)weightedVertices.data(), mesh->_vertexBufferSize*sizeof(WeightedVertex));
	}
	else
	{
		vertices.resize(mesh->_vertexBufferSize);
		_infile->read((char*)vertices.data(), mesh->_vertexBufferSize*sizeof(Vertex));
	}
	mesh->_vertexBuffer = CreateVertexBuffer(&weightedVertices, &vertices, mesh->_isSkinned);

	_infile->close();
	mesh->_meshLoaded = true;
	return true;
}

Texture* AssetManager::ScanTexture(string name)
{
	Texture* texture = new Texture;
	texture->_name = name;
	_textures->push_back(texture);
	return texture;
}

//Creates a RenderObject for the specified model without loading it
Mesh* AssetManager::ScanModel(string name)
{

	string file_path = MODEL_FOLDER_PATH;

	file_path.append(name);
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		throw std::runtime_error("Failed to open " + file_path);
	}

	int version;
	_infile->read((char*)&version, 4);

	if (version != 24 && version != 26 && version != 27 && version != 28)
	{
		throw std::runtime_error("Failed to load " + file_path + ":\nIncorrect fileversion");
	}
	Mesh* mesh = (this->*(_meshFormatVersion[version]))();

	mesh->_name = name;

	_infile->close();
/*
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


	if (renderObject->_isSkinned)
	{
		renderObject->_skeleton = LoadSkeleton(renderObject->_skeletonName);
	}
*/
	mesh->_meshLoaded = false;
	_meshes->push_back(mesh);
	return mesh;
}

//TODO: This stubborn code refuses to work when merged. I need an adult. /Rikhard
//RenderObject* AssetManager::ScanModel28()
//{
//	RenderObject* renderObject = new RenderObject;
//	int skeletonStringLength;
//	_infile->read((char*)&skeletonStringLength, 4);
//	renderObject->_mesh->_skeletonName.resize(skeletonStringLength);
//	_infile->read((char*)renderObject->_mesh->_skeletonName.data(), skeletonStringLength);
//
//	renderObject->_mesh->_isSkinned = strcmp(renderObject->_mesh->_skeletonName.data(), "Unrigged") != 0;
//
//	_infile->read((char*)&renderObject->_mesh->_toMesh, 4);
//
//	MeshHeader26 meshHeader;
//	_infile->read((char*)&meshHeader, sizeof(MeshHeader26));
//
//	if (renderObject->_mesh->_isSkinned)
//	{
//		_infile->seekg(meshHeader._numberOfVertices * sizeof(WeightedVertex), ios::cur);
//	}
//	else
//	{
//		_infile->seekg(meshHeader._numberOfVertices * sizeof(Vertex), ios::cur);
//	}
//
//	renderObject->_mesh->_pointLights.resize(meshHeader._numberPointLights);
//	_infile->read((char*)renderObject->_mesh->_pointLights.data(), sizeof(PointlightData) * meshHeader._numberPointLights);
//
//	renderObject->_mesh->_spotLights.resize(meshHeader._numberSpotLights);
//	_infile->read((char*)renderObject->_mesh->_spotLights.data(), sizeof(SpotlightData) * meshHeader._numberSpotLights);
//
//	renderObject->_mesh->_vertexBufferSize = meshHeader._numberOfVertices;
//
//	renderObject->_mesh->_hitbox = new Hitbox();
//	_infile->read((char*)renderObject->_mesh->_hitbox, sizeof(Hitbox));
//
//	return renderObject;
//}

Mesh* AssetManager::ScanModel27()
{
	Mesh* mesh = new Mesh;
	int skeletonStringLength;
	_infile->read((char*)&skeletonStringLength, 4);
	string skeletonName;
	skeletonName.resize(skeletonStringLength);
	_infile->read((char*)skeletonName.data(), skeletonStringLength);

	mesh->_isSkinned = strcmp(skeletonName.data(), "Unrigged") != 0;

	_infile->read((char*)&mesh->_toMesh, 4);

	MeshHeader26 meshHeader;
	_infile->read((char*)&meshHeader, sizeof(MeshHeader26));

	if (mesh->_isSkinned)
	{
		_infile->seekg(meshHeader._numberOfVertices * sizeof(WeightedVertex), ios::cur);
	}
	else
	{
		_infile->seekg(meshHeader._numberOfVertices * sizeof(Vertex), ios::cur);
	}

	mesh->_pointLights.resize(meshHeader._numberPointLights);
	_infile->read((char*)mesh->_pointLights.data(), sizeof(PointlightData) * meshHeader._numberPointLights);

	mesh->_spotLights.resize(meshHeader._numberSpotLights);
	_infile->read((char*)mesh->_spotLights.data(), sizeof(SpotlightData) * meshHeader._numberSpotLights);

	mesh->_vertexBufferSize = meshHeader._numberOfVertices;
	return mesh;
}

Mesh* AssetManager::ScanModel26()
{
	Mesh* mesh = new Mesh;
	int skeletonStringLength;
	_infile->read((char*)&skeletonStringLength, 4);
	string skeletonName;
	skeletonName.resize(skeletonStringLength);
	_infile->read((char*)skeletonName.data(), skeletonStringLength);

	mesh->_isSkinned = strcmp(skeletonName.data(), "Unrigged") != 0;

	_infile->read((char*)&mesh->_toMesh, 4);

	MeshHeader26 meshHeader;
	_infile->read((char*)&meshHeader, sizeof(MeshHeader26));

	if (mesh->_isSkinned)
	{
		_infile->seekg(meshHeader._numberOfVertices*sizeof(WeightedVertex), ios::cur);
	}
	else
	{
		_infile->seekg(meshHeader._numberOfVertices*sizeof(Vertex), ios::cur);
	}

	mesh->_pointLights.resize(meshHeader._numberPointLights);
	for (int i = 0; i < meshHeader._numberPointLights; i++)
	{
		_infile->read((char*)&mesh->_pointLights[i]._bone, 1);
		_infile->read((char*)&mesh->_pointLights[i]._pos, 12);
		_infile->read((char*)&mesh->_pointLights[i]._col, 12);
		_infile->read((char*)&mesh->_pointLights[i]._intensity, 4);
		mesh->_pointLights[i]._range = 100;
	}

	mesh->_spotLights.resize(meshHeader._numberSpotLights);
	for (int i = 0; i < meshHeader._numberSpotLights; i++)
	{
		_infile->read((char*)&mesh->_spotLights[i]._bone, 1);
		_infile->read((char*)&mesh->_spotLights[i]._pos, 12);
		_infile->read((char*)&mesh->_spotLights[i]._color, 12);
		_infile->read((char*)&mesh->_spotLights[i]._intensity, 4);
		_infile->read((char*)&mesh->_spotLights[i]._angle, 4);
		_infile->read((char*)&mesh->_spotLights[i]._direction, 12);
		mesh->_spotLights[i]._range = 100;
	}
	mesh->_vertexBufferSize = meshHeader._numberOfVertices;
	return mesh;
}

Mesh* AssetManager::ScanModel24()
{
	Mesh* mesh = new Mesh;
	int meshes;
	_infile->read((char*)&meshes, 4);
	meshes++;
	int skeletonStringLength;
	_infile->read((char*)&skeletonStringLength, 4);
	string skeletonName;
	skeletonName.resize(skeletonStringLength);
	_infile->read((char*)skeletonName.data(), skeletonStringLength);

	mesh->_isSkinned = skeletonName.data() == "Unrigged";
	_infile->read((char*)&mesh->_toMesh, 4);

	for (int i = 0; i < meshes; i++)
	{
		if (i == 0)
		{
			MeshHeader24 meshHeader;
			_infile->read((char*)&meshHeader, sizeof(MeshHeader24));

			_infile->seekg(meshHeader._nameLength, ios::cur);
			if (mesh->_isSkinned)
			{
				_infile->seekg(meshHeader._numberOfVertices*sizeof(WeightedVertex), ios::cur);
			}
			else
			{
				_infile->seekg(meshHeader._numberOfVertices*sizeof(Vertex), ios::cur);
			}

			mesh->_pointLights.resize(meshHeader._numberPointLights);
			for (int i = 0; i < meshHeader._numberPointLights; i++)
			{
				_infile->read((char*)&mesh->_pointLights[i]._pos, 12);
				_infile->read((char*)&mesh->_pointLights[i]._col, 12);
				_infile->read((char*)&mesh->_pointLights[i]._intensity, 4);
				mesh->_pointLights[i]._bone = 0;
				mesh->_pointLights[i]._range = 100;
			}

			mesh->_spotLights.resize(meshHeader._numberSpotLights);
			for (int i = 0; i < meshHeader._numberSpotLights; i++)
			{
				_infile->read((char*)&mesh->_spotLights[i]._pos, 12);
				_infile->read((char*)&mesh->_spotLights[i]._color, 12);
				_infile->read((char*)&mesh->_spotLights[i]._intensity, 4);
				_infile->read((char*)&mesh->_spotLights[i]._angle, 4);
				_infile->read((char*)&mesh->_spotLights[i]._direction, 12);
				mesh->_spotLights[i]._bone = 0;
				mesh->_spotLights[i]._range = 100;
			}
			mesh->_vertexBufferSize = meshHeader._numberOfVertices;
		}
		else
		{
			MeshHeader24 meshHeader;
			_infile->read((char*)&meshHeader, sizeof(MeshHeader24));

			_infile->seekg(meshHeader._nameLength, ios::cur);
			if (mesh->_isSkinned)
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
	return mesh;
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
RenderObject* AssetManager::GetRenderObject(int index)
{
	RenderObject* renderObject = nullptr;
	if (index >= 0 && index < (int)_renderObjects->size())
	{
		renderObject = _renderObjects->at(index);
		if (!renderObject->_mesh->_meshLoaded)
		{
			LoadModel(renderObject->_mesh->_name, renderObject->_mesh);
		}
		renderObject->_mesh->_activeUsers++;
	}
	return renderObject;
}

RenderObject* AssetManager::GetRenderObject(string meshName, string textureName)
{
	for (RenderObject* renderObject : *_renderObjects)
	{
		if (meshName == renderObject->_mesh->_name &&
			textureName == renderObject->_diffuseTexture->_name)
		{
			return renderObject;
		}
	}
	RenderObject* renderObject = new RenderObject;
	renderObject->_mesh = GetModel(meshName);
	renderObject->_diffuseTexture = GetTexture(textureName);
	_renderObjects->push_back(renderObject);
	return renderObject;
}

HRESULT AssetManager::ParseLevelHeader(Level::LevelHeader* outputLevelHead, std::string levelHeaderFilePath)
{
	try
	{
		std::ifstream in(levelHeaderFilePath);
		cereal::JSONInputArchive archive(in);
		archive(*outputLevelHead);
		in.close();
	}
	catch (...)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT AssetManager::ParseLevelBinary(Level::LevelBinary* outputLevelBin, std::string levelBinaryFilePath)
{
	try
	{
		std::ifstream in(levelBinaryFilePath);
		cereal::BinaryInputArchive archive(in);
		archive(*outputLevelBin);
		in.close();
	}
	catch (...)
	{
		return E_FAIL;
	}
	return S_OK;
}

Texture* AssetManager::GetTexture(string name)
{
	for (Texture* texture : *_textures)
	{
		if (texture->_name == name)
		{
			texture->_activeUsers++;
			return texture;
		}
	}
	Texture* texture = ScanTexture(name);
	texture->LoadTexture(_device);
	return texture;
}

Mesh* AssetManager::GetModel(string name)
{
	for (Mesh* mesh : *_meshes)
	{
		if (mesh->_name == name)
		{
			mesh->_activeUsers++;
			return mesh;
		}
	}
	Mesh* mesh = ScanModel(name);
	mesh->_activeUsers++;
	LoadModel(name, mesh);
	return mesh;
}

Skeleton* AssetManager::LoadSkeleton(string name)
{
	for (Skeleton* skeleton : *_skeletons)
	{
		if (strcmp(skeleton->_name.c_str(), name.data()))
		{
			return skeleton;
		}
	}

	string file_path = ANIMATION_FOLDER_PATH;

	file_path.append(name);
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