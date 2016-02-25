#include "CombinedMeshGenerator.h"

CombinedMeshGenerator::CombinedMeshGenerator(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	_device = device;
	_deviceContext = deviceContext;
}

CombinedMeshGenerator::~CombinedMeshGenerator()
{
	for (auto& meshVector : _combinedMeshes)
	{
		for (auto& mesh: meshVector)
		{
			delete mesh.combinedObject->_mesh;
		}
	}
}


/* CombineMeshes parameters explained:
Tilemap - Holds all objects in the entire map. Should be scanned for the Type which should be combined.
Type - The type of object to combine. This type can be, for example, FLOOR.
countPerCombine - Specifies how many objects that should be combined in each combinations. If this is set to -1 it should combine all objects of that type into a single mesh.
If Type == WALL and countPerCombine is 2, the walls will be combined 2 and 2. */
void CombinedMeshGenerator::CombineMeshes(Tilemap* tilemap, const Type& type, int countPerCombine)
{
	std::vector<Vertex> data;

	//Check if(renderObject1 == renderObject2), if so, combine
	int width = tilemap->GetWidth();
	int height = tilemap->GetHeight();

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			std::vector<GameObject*>* objectsOnTile = tilemap->GetAllObjectsOnTile(x, y);
		}
	}


	Mesh mesh;
}
