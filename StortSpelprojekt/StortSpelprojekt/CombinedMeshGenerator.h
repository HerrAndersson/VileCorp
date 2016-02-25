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

class CombinedMeshGenerator
{

	struct CombinedMesh
	{
		RenderObject* combinedObject; //Should be created as a new render object, but with all data copied except the vertex buffer
	};

	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;

	std::vector<std::vector<CombinedMesh>> _combinedMeshes;

	int _combinedTypes;

public:

	CombinedMeshGenerator(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~CombinedMeshGenerator();

	void CombineMeshes(Tilemap* tilemap, const Type& type, int countPerCombine = -1);


};

