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
	_meshFormatVersion[28] = &AssetManager::ScanModel28;
	_meshFormatVersion[29] = &AssetManager::ScanModel29;

	GetFilenamesInDirectory((char*)System::LEVEL_FOLDER_PATH.c_str(), ".lvl", *_levelFileNames);
}

AssetManager::~AssetManager()
{
	for (uint i = 0; i < _renderObjects->size(); i++)
	{
		delete _renderObjects->at(i);
	}
	for (Texture* texture : *_textures)
	{
		delete texture;
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
		wstring _filePath = System::TEXTURE_FOLDER_PATH_W;
		_filePath.append(_name.begin(), _name.end());
//		res = DirectX::CreateWICTextureFromFile(device, _filePath.c_str(), nullptr, &_data, 0);
//		DirectX::CreateDDSTextureFromFile(device, (_filePath.substr(0,_filePath.size()-3) + L"dds").c_str(), nullptr, &_data, 0);
		res = DirectX::CreateDDSTextureFromFileEx(device, (_filePath + L".dds").c_str(), 0, D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, 0, 0, false, nullptr, &_data,0);

		if (res != S_OK)
		{
			throw std::runtime_error("On Load Texture: " + _name + " failed");
		}
		_loaded = true;
	}
	_activeUsers++;
	return res;
}

//Loads a model to the GPU
bool AssetManager::LoadModel(const std::string& name, Mesh* mesh) 
{

	string file_path = System::MODEL_FOLDER_PATH;

	file_path.append(name);
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		return false;
	}

	vector<Vertex> vertices;
	vector<WeightedVertex> weightedVertices;

	_infile->seekg(mesh->_toMesh);
	if (mesh->_isSkinned)
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

Texture* AssetManager::ScanTexture(const std::string& name)
{
	if (name.empty())
	{
		throw exception("ScanTexture: Name is empty");
	}

	Texture* texture = new Texture;
	texture->_name = name;
	_textures->push_back(texture);
	return texture;
}

//Creates a RenderObject for the specified model without loading it
Mesh* AssetManager::ScanModel(const std::string& name)
{

	string file_path = System::MODEL_FOLDER_PATH;

	file_path.append(name);
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		throw std::runtime_error("Failed to open " + file_path);
	}

	int version;
	_infile->read((char*)&version, 4);

	if (!_meshFormatVersion.count(version))
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
		*/

	if (mesh->_isSkinned)
	{
		mesh->_skeleton = LoadSkeleton(mesh->_skeletonName);
	}

	mesh->_meshLoaded = false;
	_meshes->push_back(mesh);
	return mesh;
}

Mesh* AssetManager::ScanModel29()
{
	Mesh* mesh = new Mesh;
	bool _particles, _icon;
	_infile->read((char*)&_particles, 1);
	_infile->read((char*)&_icon, 1);
	_infile->seekg(2, std::ios::cur);

	int skeletonStringLength;
	_infile->read((char*)&skeletonStringLength, 4);
	mesh->_skeletonName.resize(skeletonStringLength);
	_infile->read((char*)mesh->_skeletonName.data(), skeletonStringLength);

	mesh->_isSkinned = strcmp(mesh->_skeletonName.data(), "Unrigged") != 0;

	_infile->read((char*)&mesh->_toMesh, 4);

	MeshHeader29 meshHeader;
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

	mesh->_hitbox = new System::Hitbox();
	_infile->read((char*)mesh->_hitbox, sizeof(System::Hitbox));
	if (_particles)
		_infile->read((char*)mesh->_particleSpawnerPos, 12);
	if (_icon)
		_infile->read((char*)mesh->_iconPos, 12);

	return mesh;
}

Mesh* AssetManager::ScanModel28()
{
	Mesh* mesh = new Mesh;
	int skeletonStringLength;
	_infile->read((char*)&skeletonStringLength, 4);
	mesh->_skeletonName.resize(skeletonStringLength);
	_infile->read((char*)mesh->_skeletonName.data(), skeletonStringLength);

	mesh->_isSkinned = strcmp(mesh->_skeletonName.data(), "Unrigged") != 0;

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

	mesh->_hitbox = new System::Hitbox();
	_infile->read((char*)mesh->_hitbox, sizeof(System::Hitbox));

	return mesh;
}

Mesh* AssetManager::ScanModel27()
{
	Mesh* mesh = new Mesh;
	int skeletonStringLength;
	_infile->read((char*)&skeletonStringLength, 4);
	mesh->_skeletonName.resize(skeletonStringLength);
	_infile->read((char*)mesh->_skeletonName.data(), skeletonStringLength);

	mesh->_isSkinned = strcmp(mesh->_skeletonName.data(), "Unrigged") != 0;

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
		auto* pointLight = &mesh->_pointLights[i];
		_infile->read((char*)&pointLight->_bone, 1);
		_infile->read((char*)&pointLight->_pos, 12);
		_infile->read((char*)&pointLight->_col, 12);
		_infile->read((char*)&pointLight->_intensity, 4);
		pointLight->_range = 100;
	}

	mesh->_spotLights.resize(meshHeader._numberSpotLights);
	for (int i = 0; i < meshHeader._numberSpotLights; i++)
	{
		auto* spotLight = &mesh->_spotLights[i];
		_infile->read((char*)&spotLight->_bone, 1);
		_infile->read((char*)&spotLight->_pos, 12);
		_infile->read((char*)&spotLight->_color, 12);
		_infile->read((char*)&spotLight->_intensity, 4);
		_infile->read((char*)&spotLight->_angle, 4);
		_infile->read((char*)&spotLight->_direction, 12);
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

	mesh->_isSkinned = skeletonName == "Unrigged";
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
				auto* pointLight = &mesh->_pointLights[i];
				_infile->read((char*)&pointLight->_pos, 12);
				_infile->read((char*)&pointLight->_col, 12);
				_infile->read((char*)&pointLight->_intensity, 4);
				pointLight->_bone = 0;
				pointLight->_range = 100;
			}

			mesh->_spotLights.resize(meshHeader._numberSpotLights);
			for (int i = 0; i < meshHeader._numberSpotLights; i++)
			{
				auto* spotLight = &mesh->_spotLights[i];
				_infile->read((char*)&spotLight->_pos, 12);
				_infile->read((char*)&spotLight->_color, 12);
				_infile->read((char*)&spotLight->_intensity, 4);
				_infile->read((char*)&spotLight->_angle, 4);
				_infile->read((char*)&spotLight->_direction, 12);
				spotLight->_bone = 0;
				spotLight->_range = 100;
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

RenderObject* AssetManager::GetRenderObject(const std::string& meshName, const std::string& textureName)
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
	renderObject->_id = _idCounter++;
	_renderObjects->push_back(renderObject);
	return renderObject;
}

HRESULT AssetManager::ParseLevelHeader(Level::LevelHeader* outputLevelHead, const std::string& levelHeaderFilePath)
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

HRESULT AssetManager::ParseLevelBinary(Level::LevelBinary* outputLevelBin, const std::string& levelBinaryFilePath)
{
	try
	{
		std::ifstream in(levelBinaryFilePath, std::ios::binary);
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

Texture* AssetManager::GetTexture(std::string name)
{
	if (name.find(".png") != std::string::npos)
	{
		name.resize(name.size() - 4);
	}
	for (Texture* texture : *_textures)
	{
		if (texture->_name == name)
		{
			texture->LoadTexture(_device);

			return texture;
		}
	}
	Texture* texture = ScanTexture(name);
	texture->LoadTexture(_device);
	texture->_id = _textureIdCounter++;
	return texture;
}

void AssetManager::Clean()
{
	for (uint i = 0; i < _textures->size(); i++)
	{
		if (!_textures->at(i)->_activeUsers)
		{
			delete _textures->at(i);
			_textures->erase(_textures->begin() + i);
		}
	}
}

Mesh* AssetManager::GetModel(const std::string& name)
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

Skeleton* AssetManager::LoadSkeleton(const std::string& name)
{
	for (Skeleton* skeleton : *_skeletons)
	{
		if (skeleton->_name == name)
		{
			return skeleton;
		}
	}

	string file_path = System::ANIMATION_FOLDER_PATH;

	file_path.append(name);
	_infile->open(file_path.c_str(), ifstream::binary);

	if (!_infile->is_open())
	{
		throw runtime_error("Failed to open " + file_path);
	}

	Skeleton* skeleton = new Skeleton;
	_skeletons->push_back(skeleton);
	skeleton->_name = name;
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
			auto& bone = skeleton->_actions[a]._bones[b];
			_infile->read((char*)&frames, 4);
			bone._frameCount = frames;
			bone._frameTime.resize(frames);
			_infile->read((char*)bone._frameTime.data(), frames * sizeof(int));
			bone._frames = (Frame*)_aligned_malloc(sizeof(Frame) * frames, 16);

			for (int i = 0; i < frames; i++)
			{
				_infile->read((char*)&translation, sizeof(XMFLOAT3));
				_infile->read((char*)&rotation, sizeof(XMFLOAT4));
				_infile->read((char*)&scale, sizeof(XMFLOAT3));
				auto& frame = bone._frames[i];
				frame._translation = XMLoadFloat3(&translation);
				frame._rotation = XMLoadFloat4(&rotation);
				frame._scale = XMLoadFloat3(&scale);
			}
		}
	}

	_infile->close();
	return skeleton;
}