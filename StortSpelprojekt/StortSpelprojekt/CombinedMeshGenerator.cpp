#include "CombinedMeshGenerator.h"
#include "stdafx.h"

using namespace DirectX;

CombinedMeshGenerator::CombinedMeshGenerator(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	_device = device;
	_deviceContext = deviceContext;
	_bufferCopy = nullptr;
	_combinedTypes = 0;
}

CombinedMeshGenerator::~CombinedMeshGenerator()
{
	for (auto& objVector : _combinedMeshes)
	{
		//Need to delete "obj" as well, but this gives unresolved externals as it deletes the texture
		for (auto& obj : objVector)
		{
			delete obj._combinedObject->_mesh;
			delete obj._combinedObject;
		}
	}

	_combinedMeshes.clear();

	SAFE_RELEASE(_bufferCopy);
}

//Copy the contents of the vertex buffer of a single object into a vector holding that data
void CombinedMeshGenerator::LoadVertexBufferData(std::vector<Vertex>* dataVector, Mesh* mesh)
{
	HRESULT result;
	dataVector->resize(mesh->_vertexBufferSize);

	SAFE_RELEASE(_bufferCopy);
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.BindFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.ByteWidth = sizeof(Vertex) *  mesh->_vertexBufferSize;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	result = _device->CreateBuffer(&bufferDesc, NULL, &_bufferCopy);
	if (FAILED(result))
	{
		throw std::runtime_error("CombinedMeshGenerator::LoadVertexBufferData: Failed to create _bufferCopy");
	}

	if (mesh->_vertexBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		_deviceContext->CopyResource(_bufferCopy, mesh->_vertexBuffer);

		result = _deviceContext->Map(_bufferCopy, 0, D3D11_MAP_READ, 0, &mappedResource);

		if (FAILED(result))
		{
			throw std::runtime_error("CombinedMeshGenerator::LoadVertexBufferData: Failed to Map _bufferCopy");
		}
		int size = sizeof(Vertex) * mesh->_vertexBufferSize;
		memcpy(dataVector->data(), mappedResource.pData, size);
		_deviceContext->Unmap(mesh->_vertexBuffer, 0);
	}
}

void CombinedMeshGenerator::CreateRenderObject(RenderObject* copy, RenderObject* orig)
{
	Mesh* mesh = new Mesh();
	mesh->_activeUsers = 1;
	mesh->_hitbox = nullptr;
	mesh->_isSkinned = false;
	mesh->_meshLoaded = true;
	mesh->_name = std::string("combined") + std::to_string(_combinedTypes);
	mesh->_pointLights.clear();
	mesh->_skeleton = nullptr;
	mesh->_skeletonName = std::string("combinedskeleton") + std::to_string(_combinedTypes);
	mesh->_spotLights.clear();
	mesh->_toMesh = 0;
	mesh->_vertexBuffer = nullptr;
	mesh->_vertexBufferSize = -1;

	copy->_mesh = mesh;
	copy->_specularTexture = orig->_specularTexture;
	copy->_diffuseTexture = orig->_diffuseTexture;
	copy->_type = orig->_type;

	if (orig->_diffuseTexture)
	{
		orig->_diffuseTexture->_activeUsers++;
	}
	if (orig->_specularTexture)
	{
		orig->_specularTexture->_activeUsers++;
	}
}

/* CombineMeshes parameters explained:
Tilemap - Holds all objects in the entire map. Should be scanned for the Type which should be combined.
Type - The type of object to combine. This type can be, for example, FLOOR.
countPerCombine - Specifies how many objects that should be combined in each combinations. If this is set to -1 it should combine all objects of that type (same mesh, texture etc) into a single mesh.
If Type == WALL and countPerCombine is 2, the walls will be combined 2 and 2. */
void CombinedMeshGenerator::CombineMeshes(Tilemap* tilemap, const Type& typeToCombine, int countPerCombine)
{
	_combinedMeshes.push_back(std::vector<CombinedMesh>());

	int width = tilemap->GetWidth();
	int height = tilemap->GetHeight();

	RenderObject* prevRenderObject = nullptr;
	RenderObject* currentRenderObject = nullptr;

	std::vector<RenderObject*> renderObjects;
	std::vector<int> numberPerRenderObject; //Holds how many objects that share a single render object. The indexes are directly mapped between renderObjects and numberPerRenderObject
	int index = -1;

	//Scan the tilemap to find how many different versions we should look for, and which these versions are. For example Type == FLOOR, and there are two different floors with different textures.
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			std::vector<GameObject*>* objectsOnTile = tilemap->GetAllObjectsOnTile(x, y);
			for (auto& object : *objectsOnTile)
			{
				if (object && object->GetType() == typeToCombine)
				{
					RenderObject* thisRenderObject = object->GetRenderObject();
					if (thisRenderObject != prevRenderObject)
					{
						prevRenderObject = thisRenderObject;
						renderObjects.push_back(thisRenderObject);
						numberPerRenderObject.push_back(0);
						index++;
					}
					numberPerRenderObject.at(index)++;
				}
			}
		}
	}

	//Scan the tilemap to find all objects of the given "subtypes" that was found above
	for (auto& renderObject : renderObjects)
	{
		//The combined object should copy everything but the vertex buffer
		RenderObject* combinedRenderObject = new RenderObject();
		CreateRenderObject(combinedRenderObject, renderObject);

		std::vector<Vertex> singleObjectDataVector;
		LoadVertexBufferData(&singleObjectDataVector, prevRenderObject->_mesh);

		std::vector<Vertex> combinedMeshDataVector;

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				std::vector<GameObject*>* objectsOnTile = tilemap->GetAllObjectsOnTile(x, y);
				for (auto& object : *objectsOnTile)
				{
					if (object && object->GetType() == typeToCombine)
					{
						XMFLOAT3 pos = object->GetPosition();
						XMMATRIX translation = XMMatrixTranslation(pos.x, pos.y, pos.z);

						//Translating the mesh data by the object translation
						for (auto& vertex : singleObjectDataVector)
						{
							Vertex vert = vertex;
							XMVECTOR posV = XMLoadFloat3(&vert._position);
							posV = XMVector3TransformCoord(posV, translation);
							XMStoreFloat3(&vert._position, posV);
							combinedMeshDataVector.push_back(vert);
						}
					}
				}
			}
		}

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(Vertex) * combinedMeshDataVector.size();
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexData;
		ZeroMemory(&vertexData, sizeof(vertexData));
		vertexData.pSysMem = combinedMeshDataVector.data();
		combinedRenderObject->_mesh->_vertexBufferSize = combinedMeshDataVector.size();

		HRESULT result = _device->CreateBuffer(&bufferDesc, &vertexData, &combinedRenderObject->_mesh->_vertexBuffer);
		if (FAILED(result))
		{
			throw std::runtime_error("CombinedMeshGenerator::CombineMeshes: Failed to create combined vertex buffer");
		}

		CombinedMesh combinedMesh;
		combinedMesh._world = XMMatrixIdentity();
		combinedMesh._combinedObject = combinedRenderObject;
		combinedMesh._vertexCount = combinedRenderObject->_mesh->_vertexBufferSize;
		combinedMesh._vertexSize = sizeof(Vertex);

		_combinedMeshes.at(_combinedTypes).push_back(combinedMesh);
	}
	_combinedTypes++;
}

std::vector<std::vector<CombinedMesh>>* CombinedMeshGenerator::GetCombinedMeshes()
{
	return &_combinedMeshes;
}
