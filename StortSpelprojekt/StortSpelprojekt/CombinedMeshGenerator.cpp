#include "CombinedMeshGenerator.h"
#include "stdafx.h"

CombinedMeshGenerator::CombinedMeshGenerator(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	_device = device;
	_deviceContext = deviceContext;
	_bufferCopy = nullptr;
}

CombinedMeshGenerator::~CombinedMeshGenerator()
{
	for (auto& meshVector : _combinedMeshes)
	{
		for (auto& mesh: meshVector)
		{
			delete mesh._combinedObject->_mesh;
		}
	}

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
		throw std::runtime_error("CombinedMeshGenerator::CombineMeshes: Failed to create _bufferCopy");
	}

	if (mesh->_vertexBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		_deviceContext->CopyResource(_bufferCopy, mesh->_vertexBuffer);

		result = _deviceContext->Map(_bufferCopy, 0, D3D11_MAP_READ, 0, &mappedResource);

		if (FAILED(result))
		{
			throw std::runtime_error("CombinedMeshGenerator::CombineMeshes: Failed to Map _bufferCopy");
		}
		int size = sizeof(Vertex) * mesh->_vertexBufferSize;
		memcpy(dataVector->data(), mappedResource.pData, size);
		_deviceContext->Unmap(mesh->_vertexBuffer, 0);
	}
}

/* CombineMeshes parameters explained:
Tilemap - Holds all objects in the entire map. Should be scanned for the Type which should be combined.
Type - The type of object to combine. This type can be, for example, FLOOR.
countPerCombine - Specifies how many objects that should be combined in each combinations. If this is set to -1 it should combine all objects of that type (same mesh, texture etc) into a single mesh.
If Type == WALL and countPerCombine is 2, the walls will be combined 2 and 2. */
void CombinedMeshGenerator::CombineMeshes(Tilemap* tilemap, const Type& typeToCombine, int countPerCombine)
{
	int width = tilemap->GetWidth();
	int height = tilemap->GetHeight();

	int subType = -1;
	RenderObject* prevRenderObject = nullptr;

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
					}
				}
			}
		}
	}

	std::vector<Vertex> dataVector;
	LoadVertexBufferData(&dataVector, prevRenderObject->_mesh);

	_combinedTypes++;
}

std::vector<std::vector<CombinedMesh>>* CombinedMeshGenerator::GetCombinedMeshes()
{
	return &_combinedMeshes;
}
