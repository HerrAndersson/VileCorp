#pragma once
#include "CommonUtils.h"
#include <vector>
#include "Tilemap.h"
#include "RenderUtils.h"
#include <d3d11.h>

/*

This class is used to combine single objects into bigger objects, to reduce the number of draw calls. This can not be done with animated objects, 
it should only be used for static objects, such as floors which there will be lots of.

The vector<vector<>> holds all combined meshes. For example, floors will be the first vector, walls (if combined) should be the next, etc

*/

//namespace dx = DirectX;

struct CombinedMesh
{
	XMMATRIX _world;
	RenderObject* _combinedObject; //Should be created as a new render object, but with all data copied except the vertex buffer
	int _vertexSize;
	int _vertexCount;
};

class CombinedMeshGenerator
{

private:

	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;
	ID3D11Buffer* _bufferCopy;

	std::vector<std::vector<bool>> _tileIsCombined; //Each position corresponds to the same position in the tilemap
	std::vector<std::vector<CombinedMesh>> _combinedMeshes;
	int _combinedTypes;

	void LoadVertexBufferData(std::vector<Vertex>* dataVector, Mesh* mesh);
	void CreateRenderObject(RenderObject* copy, RenderObject* orig);

	void CreateCombinedMesh(std::vector<Vertex>& combinedMeshDataVector, RenderObject* objectToCopy, int index);

	template<class C, class T>
	auto contains(const C& v, const T& x) -> decltype(end(v), true)
	{
		return end(v) != std::find(begin(v), end(v), x);
	}

public:

	CombinedMeshGenerator(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~CombinedMeshGenerator();

	void CombineMeshes(Tilemap* tilemap, const System::Type& typeToCombine, int countPerCombine = -1); //Adds the vertex buffers together. Can specify how many objects to add, if all should be combined, send -1
	void CombineAndOptimizeMeshes(Tilemap* tilemap, const System::Type& typeToCombine); //Scans the map to find where the vertex buffers can be optimized. For example, a patch of 4x4 floor tiles becomes a single quad with size 4x4

	std::vector<std::vector<CombinedMesh>>* GetCombinedMeshes();
};

