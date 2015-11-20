#include "AssetManager.h"


AssetManager::AssetManager(ID3D11Device* device)
{
	this->device = device;
	SetupRenderObjectList();

	//Hardcoded cube for testing. Will be removed
	{
		cube.diffuse[0] = 1;
		cube.diffuse[1] = 1;
		cube.diffuse[2] = 1;
		cube.diffuse[3] = 1;
		Mesh cubeMesh;
		cubeMesh.name = "cube";
		cubeMesh.vertexBufferSize = 36;

		vector<Point> points;
		points.resize(8);
		Point pointsf[8] =
		{
			Point{ XMFLOAT3{ -0.5,-0.5,0.5 },0,0,0,0,0,0,0,0 },
			Point{ XMFLOAT3{ 0.5,-0.5,0.5 },0,0,0,0,0,0,0,0 },
			Point{ XMFLOAT3{ -0.5,0.5,0.5 },0,0,0,0,0,0,0,0 },
			Point{ XMFLOAT3{ 0.5,0.5,0.5 },0,0,0,0,0,0,0,0 },
			Point{ XMFLOAT3{ -0.5,0.5,-0.5 },0,0,0,0,0,0,0,0 },
			Point{ XMFLOAT3{ 0.5,0.5,-0.5 },0,0,0,0,0,0,0,0 },
			Point{ XMFLOAT3{ -0.5,-0.5,-0.5 },0,0,0,0,0,0,0,0 },
			Point{ XMFLOAT3{ 0.5,-0.5,-0.5 },0,0,0,0,0,0,0,0 },
		};
		memcpy(points.data(), pointsf, sizeof(Point) * 8);
		vector<XMFLOAT3> normals;
		normals.resize(24);
		XMFLOAT3 normalsf[24] =
		{
			XMFLOAT3{ 0,0,1 },
			XMFLOAT3{ 0,0,1 },
			XMFLOAT3{ 0,0,1 },
			XMFLOAT3{ 0,0,1 },
			XMFLOAT3{ 0,1,0 },
			XMFLOAT3{ 0,1,0 },
			XMFLOAT3{ 0,1,0 },
			XMFLOAT3{ 0,1,0 },
			XMFLOAT3{ 0,0,-1 },
			XMFLOAT3{ 0,0,-1 },
			XMFLOAT3{ 0,0,-1 },
			XMFLOAT3{ 0,0,-1 },
			XMFLOAT3{ 0,-1,0 },
			XMFLOAT3{ 0,-1,0 },
			XMFLOAT3{ 0,-1,0 },
			XMFLOAT3{ 0,-1,0 },
			XMFLOAT3{ 1,0,0 },
			XMFLOAT3{ 1,0,0 },
			XMFLOAT3{ 1,0,0 },
			XMFLOAT3{ 1,0,0 },
			XMFLOAT3{ -1,0,0 },
			XMFLOAT3{ -1,0,0 },
			XMFLOAT3{ -1,0,0 },
			XMFLOAT3{ -1,0,0 },
		};
		memcpy(normals.data(), normalsf, sizeof(XMFLOAT3) * 24);
		vector<XMFLOAT2> UVs;
		UVs.resize(14);
		XMFLOAT2 UVsf[14] =
		{
			XMFLOAT2{ 0.375,0.0 },
			XMFLOAT2{ 0.625,0.0 },
			XMFLOAT2{ 0.375,0.25 },
			XMFLOAT2{ 0.625,0.25 },
			XMFLOAT2{ 0.375,0.50 },
			XMFLOAT2{ 0.625,0.50 },
			XMFLOAT2{ 0.375,0.75 },
			XMFLOAT2{ 0.625,0.75 },
			XMFLOAT2{ 0.375,1.0 },
			XMFLOAT2{ 0.625,1.0 },
			XMFLOAT2{ 0.875,0.0 },
			XMFLOAT2{ 0.875,0.25 },
			XMFLOAT2{ 0.125,0.0 },
			XMFLOAT2{ 0.125,0.25 },
		};
		memcpy(UVs.data(), UVsf, sizeof(XMFLOAT2) * 14);
		vector<XMINT3> vertexIndices;
		vertexIndices.resize(36);
		XMINT3 vertexIndicesf[36] =
		{
			XMINT3{ 1,1,1 }, XMINT3{ 2,2,2 }, XMINT3{ 3,3,3 },
			XMINT3{ 3,3,3 }, XMINT3{ 2,2,2 }, XMINT3{ 4,4,4 },
			XMINT3{ 3,3,5 }, XMINT3{ 4,4,6 }, XMINT3{ 5,5,7 },
			XMINT3{ 5,5,7 }, XMINT3{ 4,4,6 }, XMINT3{ 6,6,8 },
			XMINT3{ 5,5,9 }, XMINT3{ 6,6,10 }, XMINT3{ 7,7,11 },
			XMINT3{ 7,7,11 }, XMINT3{ 6,6,10 }, XMINT3{ 8,8,12 },
			XMINT3{ 7,7,13 }, XMINT3{ 8,8,14 }, XMINT3{ 1,9,15 },
			XMINT3{ 1,9,15 }, XMINT3{ 8,8,14 }, XMINT3{ 2,10,16 },
			XMINT3{ 2,2,17 }, XMINT3{ 8,11,18 }, XMINT3{ 4,4,19 },
			XMINT3{ 4,4,19 }, XMINT3{ 8,11,18 }, XMINT3{ 6,12,20 },
			XMINT3{ 7,13,21 }, XMINT3{ 1,1,22 }, XMINT3{ 5,14,23 },
			XMINT3{ 5,14,23 }, XMINT3{ 1,1,22 }, XMINT3{ 3,3,24 },
		};
		memcpy(vertexIndices.data(), vertexIndicesf, sizeof(XMINT3) * 36);
		cubeMesh.vertexBufferSize = vertexIndices.size();
		cubeMesh.vertexBuffer = CreateVertexBuffer(&points, &normals, &UVs, &vertexIndices, -1);
		cube.meshes.push_back(cubeMesh);
	}
}

AssetManager::~AssetManager()
{
//	for (ID3D11ShaderResourceView* t : textures) t->Release();
//	textures.clear();

	for (int i = 0; i < renderObjects.size(); i++)
	{
		UnloadModel(i, true);
		delete renderObjects[i];
	}
	renderObjects.clear();
}

//Looks through the Models folder and creates an empty RenderObject for each entry
void AssetManager::SetupRenderObjectList()
{
	GetFilenamesInDirectory("../Assets/Models", ".bin", modelFiles);
	for (int i = 0; i < modelFiles.size(); i++)
	{
		RenderObject* renderObject = ScanModel(modelFiles[i]);
		renderObjects.push_back(renderObject);
	}
}

//Marks a model for unloading if running out of memory. Set force to true to unload now
void AssetManager::UnloadModel(int index, bool force)
{
	RenderObject* renderObject = renderObjects[index];
	if (force)
	{
		for (auto m : renderObject->meshes)
			m.vertexBuffer->Release();
		renderObject->toUnload = false;
	}
	else
	{
		renderObjectsToFlush.push_back(renderObject);
		renderObject->toUnload = true;
	}
}

//Unloads all models waiting to be unloaded
void AssetManager::Flush()
{
	for (RenderObject* renderObject : renderObjectsToFlush)
		if (renderObject->toUnload)
		{
			for (auto m : renderObject->meshes)
				m.vertexBuffer->Release();
			renderObject->toUnload = false;
		}
	renderObjectsToFlush.clear();
}

//Loads a model to the GPU
void AssetManager::LoadModel(string file_path, RenderObject* renderObject) {
	infile.open(file_path.c_str(), ifstream::binary);
	if (!infile.is_open()) { return; }

	MainHeader mainHeader;
	infile.read((char*)&mainHeader, sizeof(MainHeader));

	vector<Point> points;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT2> UVs;
	vector<XMINT3> vertexIndices;

	for (int i = 0; i < mainHeader.meshCount; i++) {
		MeshHeader meshHeader;
		infile.read((char*)&meshHeader, sizeof(MeshHeader));

		renderObject->meshes[i].name.resize(meshHeader.nameLength);
		points.resize(meshHeader.numberPoints);
		normals.resize(meshHeader.numberNormals);
		UVs.resize(meshHeader.numberCoords);
		vertexIndices.resize(meshHeader.numberFaces * 3);

		infile.read((char*)renderObject->meshes[i].name.data(), meshHeader.nameLength);
		infile.read((char*)points.data(), meshHeader.numberPoints*sizeof(Point));
		infile.read((char*)normals.data(), meshHeader.numberNormals*sizeof(XMFLOAT3));
		infile.read((char*)UVs.data(), meshHeader.numberCoords*sizeof(XMFLOAT2));
		infile.read((char*)vertexIndices.data(), meshHeader.numberFaces*sizeof(XMINT3) * 3);

		renderObject->meshes[i].vertexBufferSize = vertexIndices.size();
		renderObject->meshes[i].vertexBuffer = CreateVertexBuffer(&points, &normals, &UVs, &vertexIndices, renderObject->skeleton);
	}
	infile.close();
}

//Creates a RenderObject for the specified model without loading it
RenderObject* AssetManager::ScanModel(string file_path) {
	RenderObject* renderObject = new RenderObject;
	infile.open(file_path.c_str(), ifstream::binary);
	if (!infile.is_open())
	{
		return renderObject;
	}

	MainHeader mainHeader;
	infile.read((char*)&mainHeader, sizeof(MainHeader));

	for (int i = 0; i < mainHeader.meshCount; i++) {
		Mesh mesh;
		MeshHeader meshHeader;
		infile.read((char*)&meshHeader, sizeof(MeshHeader));

		mesh.name.resize(meshHeader.nameLength);

		infile.read((char*)mesh.name.data(), meshHeader.nameLength);
		infile.seekg(meshHeader.numberPoints*sizeof(Point));
		infile.seekg(meshHeader.numberNormals*sizeof(XMFLOAT3));
		infile.seekg(meshHeader.numberCoords*sizeof(XMFLOAT2));
		infile.seekg(meshHeader.numberFaces*sizeof(XMINT3) * 3);

		if (meshHeader.numberPointLights)
		{
			mesh.pointLights.resize(sizeof(PointLight)*meshHeader.numberPointLights);
			infile.read((char*)mesh.pointLights.data(), sizeof(PointLight)*meshHeader.numberPointLights);
		}

		if (meshHeader.numberSpotLights)
		{
			mesh.spotLights.resize(sizeof(SpotLight)*meshHeader.numberSpotLights);
			infile.read((char*)mesh.spotLights.data(), sizeof(SpotLight)*meshHeader.numberSpotLights);
		}
		mesh.vertexBufferSize = meshHeader.numberFaces * 3;
		renderObject->meshes.push_back(mesh);
	}

	MatHeader matHeader;
	infile.read((char*)&matHeader, sizeof(MatHeader));
	renderObject->diffFile.resize(matHeader.diffuseNameLength);
	renderObject->specFile.resize(matHeader.specularNameLength);
	infile.read((char*)&renderObject->diffuse, 16);
	infile.read((char*)renderObject->diffFile.data(), matHeader.diffuseNameLength);
	infile.read((char*)&renderObject->specular, 16);
	infile.read((char*)renderObject->diffFile.data(), matHeader.diffuseNameLength);

	infile.close();

	renderObject->meshLoaded = false;

	return renderObject;
}

//Unpacks an indexed mesh and sends it to the GPU, returning the resulting buffer. Will not remain in this state TODO Either proper indexbuffering or keep unpacked models as files and load them directly - Fredrik
ID3D11Buffer* AssetManager::CreateVertexBuffer(vector<Point> *points, vector<XMFLOAT3> *normals, vector<XMFLOAT2> *UVs, vector<XMINT3> *vertexIndices, int skeleton)
{
	vector<WeightedVertex> weightedVertices;
	vector<Vertex> vertices;

	if (skeleton != -1) {
		for (int i = 0; i < (signed)vertexIndices->size(); i += 3) {
			for (int a = 0; a < 3; a++) {
				WeightedVertex tempVertex;
				tempVertex.position = points->at(vertexIndices->at(i + a).x).position;
				tempVertex.normal = normals->at(vertexIndices->at(i + a).y);
				tempVertex.uv = UVs->at(vertexIndices->at(i + a).z);
				for (int b = 0; b < 4; b++)
				{
					tempVertex.boneIndices[b] = points->at(vertexIndices->at(i + a).x).boneIndices[b];
					tempVertex.boneWeights[b] = points->at(vertexIndices->at(i + a).x).boneWeights[b];
				}
				weightedVertices.push_back(tempVertex);
			}
		}
	}
	else {
		for (int i = 0; i < (signed)vertexIndices->size(); i += 3) {
			for (int a = 0; a < 3; a++) {
				Vertex tempVertex;
				tempVertex.position = points->at(vertexIndices->at(i + a).x).position;
				tempVertex.normal = normals->at(vertexIndices->at(i + a).y);
				tempVertex.uv = UVs->at(vertexIndices->at(i + a).z);
				vertices.push_back(tempVertex);
			}
		}
	}

	D3D11_BUFFER_DESC vbDESC;
	vbDESC.Usage = D3D11_USAGE_DEFAULT;
	if (skeleton != -1)
		vbDESC.ByteWidth = sizeof(WeightedVertex)* vertexIndices->size();
	else
		vbDESC.ByteWidth = sizeof(Vertex)* vertexIndices->size();
	vbDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDESC.CPUAccessFlags = 0;
	vbDESC.MiscFlags = 0;
	vbDESC.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;

	if (skeleton != -1)
		vertexData.pSysMem = weightedVertices.data();
	else
		vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Buffer* vertexBuffer;

	HRESULT result = device->CreateBuffer(&vbDESC, &vertexData, &vertexBuffer);
	if (result == E_OUTOFMEMORY) {
		Flush();
		result = device->CreateBuffer(&vbDESC, &vertexData, &vertexBuffer);
	}

	return vertexBuffer;
}

//How AssetManager interfaces with the renderer. Don't save the return, request it anew everytime unless you are certain the model won't be unloaded
//Use -1 for hardcoded cube. Will be removed
RenderObject* AssetManager::GetRenderObject(int index)
{
	if (index == -1)
		return &cube;
	RenderObject* renderObject = renderObjects[index];
	if (!renderObject->meshLoaded)
		LoadModel(modelFiles[index], renderObject);
	else if (renderObject->toUnload)
		renderObject->toUnload = false;
	return renderObject;
}