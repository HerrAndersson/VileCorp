#include "AssetManager.h"


AssetManager::AssetManager(ID3D11Device* device)
{
	this->device = device;
	SetupRenderObjectList();

	this->infile = new ifstream;
	this->modelFiles = new vector<string>;
	this->renderObjects = new vector<RenderObject*>;
	this->renderObjectsToFlush = new vector<RenderObject*>;

}

AssetManager::~AssetManager()
{
//	for (ID3D11ShaderResourceView* t : textures) t->Release();
//	textures.clear();

	for (int i = 0; i < renderObjects->size(); i++)
	{
		UnloadModel(i, true);
		delete renderObjects->at(i);
	}
	renderObjects->clear();
}

//Looks through the Models folder and creates an empty RenderObject for each entry
void AssetManager::SetupRenderObjectList()
{
	GetFilenamesInDirectory("../Assets/Models", ".bin", *modelFiles);
	for (int i = 0; i < modelFiles->size(); i++)
	{
		RenderObject* renderObject = ScanModel(modelFiles->at(i));
		renderObjects->push_back(renderObject);
	}
}

//Marks a model for unloading if running out of memory. Set force to true to unload now
void AssetManager::UnloadModel(int index, bool force)
{
	RenderObject* renderObject = renderObjects->at(index);
	if (force)
	{
		for (auto m : renderObject->meshes)
			m.vertexBuffer->Release();
		renderObject->toUnload = false;
	}
	else
	{
		renderObjectsToFlush->push_back(renderObject);
		renderObject->toUnload = true;
	}
}

//Unloads all models waiting to be unloaded
void AssetManager::Flush()
{
	for (RenderObject* renderObject : *renderObjectsToFlush)
		if (renderObject->toUnload)
		{
			for (auto m : renderObject->meshes)
				m.vertexBuffer->Release();
			renderObject->toUnload = false;
		}
	renderObjectsToFlush->clear();
}

//Loads a model to the GPU
void AssetManager::LoadModel(string file_path, RenderObject* renderObject) {
	infile->open(file_path.c_str(), ifstream::binary);
	if (!infile->is_open()) { return; }

	MainHeader mainHeader;
	infile->read((char*)&mainHeader, sizeof(MainHeader));
	infile->seekg(mainHeader.meshCount * 4);
	vector<Vertex> vertices;

	for (int i = 0; i < mainHeader.meshCount; i++) {
		MeshHeader meshHeader;
		infile->read((char*)&meshHeader, sizeof(MeshHeader));

		renderObject->meshes[i].name.resize(meshHeader.nameLength);
		vertices.resize(meshHeader.numberOfVertices);

		infile->seekg(meshHeader.nameLength);
		infile->read((char*)vertices.data(), meshHeader.numberOfVertices*sizeof(Vertex));

		renderObject->meshes[i].vertexBufferSize = vertices.size();
		renderObject->meshes[i].vertexBuffer = CreateVertexBuffer(&vertices, renderObject->skeleton);
	}
	infile->close();
}

//Creates a RenderObject for the specified model without loading it
RenderObject* AssetManager::ScanModel(string file_path)
{
	RenderObject* renderObject = new RenderObject;
	infile->open(file_path.c_str(), ifstream::binary);
	if (!infile->is_open())
	{
		return renderObject;
	}

	MainHeader mainHeader;
	infile->read((char*)&mainHeader, sizeof(MainHeader));
	renderObject->toMesh.resize(mainHeader.meshCount);

	for (int i = 0; i < mainHeader.meshCount; i++)
	{
		Mesh mesh;
		infile->read((char*)renderObject->toMesh[i], 4);
		renderObject->meshes.push_back(mesh);
	}
	for (int i = 0; i < mainHeader.meshCount; i++)
	{
		MeshHeader meshHeader;
		infile->read((char*)&meshHeader, sizeof(MeshHeader));

		renderObject->meshes[i].name.resize(meshHeader.nameLength);

		infile->read((char*)renderObject->meshes[i].name.data(), meshHeader.nameLength);
		infile->seekg(meshHeader.numberOfVertices*sizeof(Vertex));

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
	renderObject->diffFile.resize(matHeader.diffuseNameLength);
	renderObject->specFile.resize(matHeader.specularNameLength);
	infile->read((char*)&renderObject->diffuse, 16);
	infile->read((char*)&renderObject->specular, 16);
	infile->read((char*)renderObject->diffFile.data(), matHeader.diffuseNameLength);
	infile->read((char*)renderObject->diffFile.data(), matHeader.diffuseNameLength);

	infile->close();

	renderObject->meshLoaded = false;

	return renderObject;
}

//Unpacks an indexed mesh and sends it to the GPU, returning the resulting buffer. Will not remain in this state TODO Either proper indexbuffering or keep unpacked models as files and load them directly - Fredrik
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
RenderObject* AssetManager::GetRenderObject(int index)
{
	RenderObject* renderObject = renderObjects->at(index);
	if (!renderObject->meshLoaded)
		LoadModel(modelFiles->at(index), renderObject);
	else if (renderObject->toUnload)
		renderObject->toUnload = false;
	return renderObject;
}