#include "AssetManager.h"


AssetManager::AssetManager(ID3D11Device* device)
{

	this->device = device;
	this->infile = new ifstream;
	this->modelFiles = new vector<string>;
	this->renderObjects = new vector<RenderObject*>;
	this->renderObjectsToFlush = new vector<RenderObject*>;
	this->textures = new vector<Texture*>;
	this->texturesToFlush = new vector<Texture*>;

	SetupRenderObjectList();
}

AssetManager::~AssetManager()
{
	for (Texture* texture : *textures)
		if(texture->loaded)
			texture->data->Release();
	textures->clear();

	for (uint i = 0; i < renderObjects->size(); i++)
	{
		UnloadModel(i, true);
		delete renderObjects->at(i);
	}
	renderObjects->clear();
}

//Looks through the Models folder and creates an empty RenderObject for each entry
void AssetManager::SetupRenderObjectList()
{
	GetFilenamesInDirectory("../../Output/Bin/x86/Debug/Assets/Models/", ".bin", *modelFiles);//TODO should be relative to executable - Fredrik
	for (uint i = 0; i < modelFiles->size(); i++)
	{
		RenderObject* renderObject = ScanModel(modelFiles->at(i));
		renderObjects->push_back(renderObject);
	}
}

void AssetManager::DecrementUsers(Texture* texture)
{
	texture->activeUsers--;
	if (!texture->activeUsers)
		texturesToFlush->push_back(texture);
}

//Marks a model for unloading if running out of memory. Set force to true to unload now
void AssetManager::UnloadModel(int index, bool force)
{
	RenderObject* renderObject = renderObjects->at(index);
	if (!renderObject->meshLoaded)
		return;
	if (force)
	{
		for (auto m : renderObject->meshes)
			m.vertexBuffer->Release();
		renderObject->toUnload = false;
		if (renderObject->diffuseTexture != nullptr)
			DecrementUsers(renderObject->diffuseTexture);
		if (renderObject->specularTexture != nullptr)
			DecrementUsers(renderObject->specularTexture);
	}
	else
	{
		renderObjectsToFlush->push_back(renderObject);
		renderObject->toUnload = true;
	}
}

//Unloads all Assets waiting to be unloaded
void AssetManager::Flush()
{
	for (RenderObject* renderObject : *renderObjectsToFlush)
		if (renderObject->toUnload)
		{
			for (auto m : renderObject->meshes)
				m.vertexBuffer->Release();
			renderObject->toUnload = false;
			if (renderObject->diffuseTexture != nullptr)
				DecrementUsers(renderObject->diffuseTexture);
			if (renderObject->specularTexture != nullptr)
				DecrementUsers(renderObject->specularTexture);
		}
	renderObjectsToFlush->clear();

	for (Texture* texture : *textures)
		if (!texture->activeUsers)
			texture->data->Release();
	texturesToFlush->clear();
}

void Texture::LoadTexture()
{
	if (!loaded)
	{
		//TODO implement loading - Fredrik
	}
	activeUsers++;
}

//Loads a model to the GPU
void AssetManager::LoadModel(string file_path, RenderObject* renderObject) {
	infile->open(file_path.c_str(), ifstream::binary);
	if (!infile->is_open()) { return; }
	Mesh* mesh;
	vector<Vertex> vertices;

	for (uint i = 0; i < renderObject->meshes.size(); i++) {
		mesh = &renderObject->meshes[i];
		infile->seekg(mesh->toMesh);
		vertices.resize(mesh->vertexBufferSize);
		infile->read((char*)vertices.data(), mesh->vertexBufferSize*sizeof(Vertex));
		mesh->vertexBuffer = CreateVertexBuffer(&vertices, renderObject->skeleton);
	}

	if (renderObject->diffuseTexture != nullptr)
		renderObject->diffuseTexture->LoadTexture();
	if (renderObject->specularTexture != nullptr)
		renderObject->specularTexture->LoadTexture();

	infile->close();
}

Texture* AssetManager::ScanTexture(string filename)
{
	for (Texture* texture : *textures)
	{
		if (!strcmp(texture->filename.data(), filename.data()))
			return texture;
	}
	Texture* texture = new Texture;
	texture->filename = filename;
	textures->push_back(texture);
	return texture;
}

//Creates a RenderObject for the specified model without loading it
RenderObject* AssetManager::ScanModel(string file_path)
{
	RenderObject* renderObject = new RenderObject;
	infile->open(file_path.c_str(), ifstream::binary);
	if (!infile->is_open()) { return renderObject; }
	MainHeader mainHeader;
	infile->read((char*)&mainHeader, sizeof(MainHeader));
	mainHeader.meshCount++;
	if (mainHeader.version != meshFormatVersion)
		throw std::runtime_error("Incorrect fileversion");
	for (int i = 0; i < mainHeader.meshCount; i++)
	{
		Mesh mesh;
		infile->read((char*)&mesh.toMesh, 4);
		renderObject->meshes.push_back(mesh);
	}

	for (int i = 0; i < mainHeader.meshCount; i++)
	{
		MeshHeader meshHeader;
		infile->read((char*)&meshHeader, sizeof(MeshHeader));

		renderObject->meshes[i].name.resize(meshHeader.nameLength);

		infile->read((char*)renderObject->meshes[i].name.data(), meshHeader.nameLength);
		infile->seekg(meshHeader.numberOfVertices*sizeof(Vertex), ios::cur);

		if (meshHeader.numberPointLights)
		{
			renderObject->meshes[i].pointLights.resize(sizeof(PointLight)*meshHeader.numberPointLights);
			infile->read((char*)renderObject->meshes[i].pointLights.data(), sizeof(PointLight)*meshHeader.numberPointLights);
		}

		if (meshHeader.numberSpotLights)
		{
			renderObject->meshes[i].spotLights.resize(sizeof(SpotLight)*meshHeader.numberSpotLights);
			infile->read((char*)renderObject->meshes[i].spotLights.data(), sizeof(SpotLight)*meshHeader.numberSpotLights);
		}
		renderObject->meshes[i].vertexBufferSize = meshHeader.numberOfVertices;
	}
	MatHeader matHeader;
	infile->read((char*)&matHeader, sizeof(MatHeader));
	infile->read((char*)&renderObject->diffuse, 16);
	infile->read((char*)&renderObject->specular, 16);

	string diffFile, specFile;
	diffFile.resize(matHeader.diffuseNameLength);
	specFile.resize(matHeader.specularNameLength);
	infile->read((char*)diffFile.data(), matHeader.diffuseNameLength);
	infile->read((char*)specFile.data(), matHeader.specularNameLength);

	if (matHeader.diffuseNameLength)
		renderObject->diffuseTexture = ScanTexture(diffFile);
	if (matHeader.specularNameLength)
		renderObject->specularTexture = ScanTexture(specFile);

	infile->close();

	renderObject->meshLoaded = false;

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

	HRESULT result = device->CreateBuffer(&vbDESC, &vertexData, &vertexBuffer);
	if (result == E_OUTOFMEMORY) {
		Flush();
		result = device->CreateBuffer(&vbDESC, &vertexData, &vertexBuffer); // TODO handle if still out of memory - Fredrik
	}

	return vertexBuffer;
}

//How AssetManager interfaces with the renderer. Don't save the return, request it anew everytime unless you are certain the model won't be unloaded
//currently two objects exist. 0 = cube, 1 = cube with arms
RenderObject* AssetManager::GetRenderObject(int index)
{
	RenderObject* renderObject = renderObjects->at(index);
	if (!renderObject->meshLoaded)
		LoadModel(modelFiles->at(index), renderObject);
	else if (renderObject->toUnload)
		renderObject->toUnload = false;
	return renderObject;
}
