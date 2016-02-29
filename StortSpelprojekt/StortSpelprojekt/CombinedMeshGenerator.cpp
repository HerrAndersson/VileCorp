#include "CombinedMeshGenerator.h"
#include "stdafx.h"

using namespace System;
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
			SAFE_DELETE(obj._combinedObject->_mesh);
			obj._combinedObject->_diffuseTexture = nullptr;
			obj._combinedObject->_specularTexture = nullptr;
			obj._combinedObject->_mesh = nullptr;
			SAFE_DELETE(obj._combinedObject);
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

void CombinedMeshGenerator::CreateCombinedMesh(std::vector<Vertex>& combinedMeshDataVector, RenderObject* objectToCopy, int index)
{
	//The combined object should copy everything but the vertex buffer
	RenderObject* combinedRenderObject = new RenderObject();
	CreateRenderObject(combinedRenderObject, objectToCopy);




	//Combine vertex buffer data here?


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
	RenderObject* thisRenderObject = nullptr;

	std::vector<RenderObject*> renderObjects;
	std::vector<int> numberPerRenderObject; //Holds how many objects that share a single render object. The indexes are directly mapped between renderObjects and numberPerRenderObject
	int index = -1;

	//Create a map where each position corresponds to the same position in the tilemap
	std::vector<std::vector<bool>> tileIsCombined;
	tileIsCombined.resize(width);
	for (int i = 0; i < width; i++)
	{
		tileIsCombined[i].resize(height);
	}
	
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
					thisRenderObject = object->GetRenderObject();

					if(!contains(renderObjects, thisRenderObject))
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
	int combinedCount = 0;
	for (auto& renderObject : renderObjects)
	{
		std::vector<Vertex> singleObjectDataVector;
		LoadVertexBufferData(&singleObjectDataVector, prevRenderObject->_mesh);

		std::vector<Vertex> combinedMeshDataVector;

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if (!tileIsCombined[x][y])
				{
					std::vector<GameObject*>* objectsOnTile = tilemap->GetAllObjectsOnTile(x, y);
					for (auto& object : *objectsOnTile)
					{
						if (object && object->GetType() == typeToCombine && *object->GetRenderObject() == *renderObject)
						{
							XMFLOAT3 basePos = object->GetPosition();
							XMFLOAT3 endPos; //Holds the corner furthest away from basePos

							int offsetX = 0;
							int offsetY = 0;
							bool stopX = false;
							bool stopY = false;
							int maxX = std::numeric_limits<int>::infinity();
							int maxY = std::numeric_limits<int>::infinity();
							bool foundX = false;
							bool foundY = false;

							while (!stopX && (x + offsetX) < width)
							{

								std::vector<GameObject*>* xObjectsOnTile = tilemap->GetAllObjectsOnTile(x + offsetX, y);
								for (auto& xObject : *xObjectsOnTile)
								{
									if (xObject && xObject->GetType() == typeToCombine && *xObject->GetRenderObject() == *renderObject)
									{
										foundX = true;
									}
								}

								if (foundX)
								{
									while (!stopY && (y + offsetY) < height)
									{
										foundY = false;
										if (!tileIsCombined[x + offsetX][y + offsetY])
										{
											std::vector<GameObject*>* yObjectsOnTile = tilemap->GetAllObjectsOnTile(x + offsetX, y + offsetY);
											for (auto& yObject : *yObjectsOnTile)
											{
												if (yObject && yObject->GetType() == typeToCombine && *yObject->GetRenderObject() == *renderObject)
												{
													tileIsCombined[x + offsetX][y + offsetY] = true;
													foundY = true;
												}
											}
										}
										else
										{
											maxY = y + offsetY - 1;
											stopY = true;
											break;
										}

										if (!foundY)
										{
											maxY = y + offsetY - 1;
											stopY = true;
											break;
										}

										offsetY++;
									}
								}
								else
								{
									stopX = true;
									maxX = x + offsetX - 1;
									break;
								}

								//Restart the next column
								stopY = false;
								offsetX++;
								offsetY = 0;
								foundX = false;
								foundY = false;
							}

							float scaleX = maxX-basePos.x+1;
							float scaleY = maxY-basePos.z+1;

							XMMATRIX scaleTranslation = XMMatrixScaling(scaleX, 1.0f, scaleY);
							XMMATRIX scaleUV = XMMatrixScaling(scaleX, scaleY, 1.0f);
							XMMATRIX translation = scaleTranslation * XMMatrixTranslation(basePos.x + scaleX/2.0f-0.5f, basePos.y, basePos.z + scaleY/2.0f-0.5f);

							//Translating the mesh data by the object translation
							for (auto& vertex : singleObjectDataVector)
							{
								Vertex vert = vertex;

								//Transform the position
								XMVECTOR posV = XMLoadFloat3(&vert._position);
								posV = XMVector3TransformCoord(posV, translation);
								XMStoreFloat3(&vert._position, posV);

								//Scale the uv-coords according to the same scaling as the position
								XMVECTOR uvV = XMLoadFloat2(&vert._uv);
								uvV = XMVector2TransformCoord(uvV, scaleUV);
								XMStoreFloat2(&vert._uv, uvV);

								combinedMeshDataVector.push_back(vert);
							}

							combinedCount++;

							if (countPerCombine != -1 && combinedCount % countPerCombine == 0)
							{
								//Create a combination of a given number of meshes
								CreateCombinedMesh(combinedMeshDataVector, renderObject, _combinedTypes);
								combinedMeshDataVector.clear();
							}
						}
					}
				}
			}
		}

		if (countPerCombine != -1 && combinedMeshDataVector.size() > 0)
		{
			//If combinedCount % countPerCombine is not 0 the last time, the remaining objects will have to be added here. 
			//For example, countPerCombine is 4, but there are 10 objects. The last two will be combined here
			CreateCombinedMesh(combinedMeshDataVector, renderObject, _combinedTypes);
			combinedMeshDataVector.clear();
		}

		if (countPerCombine == -1)
		{
			//Combine all meshes of the give type
			CreateCombinedMesh(combinedMeshDataVector, renderObject, _combinedTypes);
			combinedMeshDataVector.clear();
		}
	}

	_combinedTypes++;
}

std::vector<std::vector<CombinedMesh>>* CombinedMeshGenerator::GetCombinedMeshes()
{
	return &_combinedMeshes;
}
