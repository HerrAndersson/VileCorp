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
	Release();
}

void CombinedMeshGenerator::Release()
{
	for (auto& objVector : _combinedMeshes)
	{
		for (auto& obj : objVector)
		{
			SAFE_DELETE(obj._combinedObject->_mesh);
			obj._combinedObject->_diffuseTexture = nullptr;
			obj._combinedObject->_specularTexture = nullptr;
			obj._combinedObject->_mesh = nullptr;
			SAFE_DELETE(obj._combinedObject);
		}
	}
	SAFE_RELEASE(_bufferCopy);
	_combinedMeshes.clear();

	for (unsigned int i = 0; i < _tileIsCombined.size(); i++)
	{
		_tileIsCombined[i].clear();
	}
	_tileIsCombined.clear();
}

void CombinedMeshGenerator::Reset()
{
	Release();
	_bufferCopy = nullptr;
	_combinedTypes = 0;
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
	if (combinedMeshDataVector.size() > 0)
	{
		//The combined object should copy everything but the vertex buffer
		RenderObject* combinedRenderObject = new RenderObject();
		CreateRenderObject(combinedRenderObject, objectToCopy);

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
}

/* CombineMeshes parameters explained:
Tilemap - Holds all objects in the entire map. Should be scanned for the Type which should be combined.
Type - The type of object to combine. This type can be, for example, FLOOR.*/
void CombinedMeshGenerator::CombineAndOptimizeMeshes(Tilemap* tilemap, const Type& typeToCombine)
{
	_combinedMeshes.push_back(std::vector<CombinedMesh>());

	int width = tilemap->GetWidth();
	int height = tilemap->GetHeight();

	RenderObject* prevRenderObject = nullptr;
	RenderObject* thisRenderObject = nullptr;

	std::vector<RenderObject*> renderObjects;
	std::vector<int> numberPerRenderObject; //Holds how many objects that share a single render object. The indexes are directly mapped between renderObjects and numberPerRenderObject
	int index = -1;

	if (_tileIsCombined.size() <= 0)
	{
		_tileIsCombined.resize(width);
		for (int i = 0; i < width; i++)
		{
			_tileIsCombined[i].resize(height);
		}
	}
	
	//Scan the tilemap to find how many different versions we should look for, and which these versions are. For example Type == FLOOR, and there are two different floors with different textures.
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			GameObject* object = tilemap->GetObjectOnTile(x, y, typeToCombine);
			if (object)
			{
				thisRenderObject = object->GetRenderObject();

				bool found = false;
				for (auto& renderObject : renderObjects)
				{
					if (*renderObject == *thisRenderObject)
					{
						found = true;
						break;
					}
				}

				if (!found)
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

	//Scan the tilemap to find all objects of the given "subtypes" that was found above
	int combinedCount = 0;
	for (auto& renderObject : renderObjects)
	{
		std::vector<Vertex> combinedMeshDataVector;

		std::vector<Vertex> singleObjectDataVector;
		LoadVertexBufferData(&singleObjectDataVector, renderObject->_mesh);

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				GameObject* object = tilemap->GetObjectOnTile(x, y, typeToCombine);
				if (object && !_tileIsCombined[x][y])
				{
					if (*object->GetRenderObject() == *renderObject)
					{
						XMFLOAT3 basePos = object->GetPosition();
						int offsetX = 0;
						int offsetY = 1;
						bool stopX = false;
						int maxX = 999999999;
						int maxY = 999999999;
						bool foundX = false;
						bool foundY = false;
						XMFLOAT3 pos;

						//Check X-direction
						while (!stopX && (x + offsetX) < width)
						{
							//Check if the tile holds an object of the type that is being combined
							GameObject* xObject = tilemap->GetObjectOnTile(x + offsetX, y, typeToCombine);
							if (xObject && *xObject->GetRenderObject() == *renderObject && !_tileIsCombined[x + offsetX][y])
							{
								foundX = true;
								_tileIsCombined[x + offsetX][y] = true;

								//Check Y-direction of this x-position
								while ((y + offsetY) < height)
								{
									GameObject* yObject = tilemap->GetObjectOnTile(x + offsetX, y + offsetY, typeToCombine);
									if (yObject && *yObject->GetRenderObject() == *renderObject && !_tileIsCombined[x + offsetX][y+offsetY])
									{
										pos = yObject->GetPosition();
										foundY = true;
									}

									if (!foundY)
									{
										if (pos.z <= maxY)
										{
											maxY = y + offsetY - 1;
										}

										if (offsetY == 1)
										{
											maxY = y;
										}

										break;
									}

									offsetY++;
									foundY = false;
								}
							}


							//If a tile does not hold an object of the type, the max value for x has been found
							if (!foundX)
							{
								stopX = true;
								maxX = x + offsetX - 1;
								break;
							}

							offsetY = 1;
							foundY = false;
							offsetX++;
							foundX = false;
						}

						//Mark the found tiles as combined
						for (int xo = x; xo < maxX+1 && xo < width; xo++)
						{
							for (int yo = y; yo < maxY+1 && yo < height; yo++)
							{
								_tileIsCombined[xo][yo] = true;
							}
						}

						float scaleX = maxX - basePos.x + 1;
						float scaleY = maxY - basePos.z + 1;

						XMMATRIX scaleTranslation = XMMatrixScaling(scaleX, 1.0f, scaleY);
						XMMATRIX scaleUV = XMMatrixScaling(scaleX, scaleY, 1.0f);
						 
						XMMATRIX translation = scaleTranslation * XMMatrixTranslation(basePos.x + scaleX / 2.0f - 0.5f, basePos.y, basePos.z + scaleY / 2.0f - 0.5f);

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
					}
				}
			}
		}

		//Combine all meshes of the give type
		CreateCombinedMesh(combinedMeshDataVector, renderObject, _combinedTypes);
		combinedMeshDataVector.clear();
	}

	_combinedTypes++;
}

//Combines by just adding the vertex buffers. Not the best solution, but it reduces the draw calls
void CombinedMeshGenerator::CombineMeshes(Tilemap* tilemap, const System::Type& typeToCombine, int countPerCombine)
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
			GameObject* object = tilemap->GetObjectOnTile(x, y, typeToCombine);
			if (object && object->GetType() == typeToCombine)
			{
				RenderObject* thisRenderObject = object->GetRenderObject();

				bool found = false;
				for (auto& renderObject : renderObjects)
				{
					if (*renderObject == *thisRenderObject)
					{
						found = true;
						break;
					}
				}

				if (!found)
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

	//Scan the tilemap to find all objects of the given "subtypes" that was found above
	int combinedCount = 0;
	for (auto& renderObject : renderObjects)
	{
		std::vector<Vertex> combinedMeshDataVector;
		std::vector<Vertex> singleObjectDataVector;
		LoadVertexBufferData(&singleObjectDataVector, renderObject->_mesh);

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				GameObject* object = tilemap->GetObjectOnTile(x, y, typeToCombine);

				if (object && object->GetType() == typeToCombine && *object->GetRenderObject() == *renderObject)
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

		//If combinedCount % countPerCombine is not 0 the last time, the remaining objects will have to be added here. 
		//For example, countPerCombine is 4, but there are 10 objects. The last two will be combined here
		if (countPerCombine != -1 && combinedMeshDataVector.size() > 0)
		{
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
