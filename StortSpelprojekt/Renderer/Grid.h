#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <stdexcept>

//Disable warning about dll-interface
#pragma warning( disable: 4251 )

class __declspec(dllexport) Grid
{
private:

	ID3D11Buffer* _lineBuffer;

	float _gridOffset;
	int _gridSizeX;
	int _gridSizeY;
	int _vertexSize;
	int _nrOfPoints;

	DirectX::XMFLOAT3 _colorOffset;

	std::vector<DirectX::XMMATRIX> _gridMatrices;

	void CreateGrid();

public:

	Grid(ID3D11Device* device, float gridOffset, int gridSizeX, int gridSizeY, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0,0,0));
	~Grid();

	std::vector<DirectX::XMMATRIX>* GetGridMatrices();
	ID3D11Buffer* GetLineBuffer();

	int GetVertexSize() const;
	int GetNrOfPoints() const;
	DirectX::XMFLOAT3 GetColorOffset() const;

	void ChangeGridSize(int gridSizeX, int gridSizeY, int gridOffset);

	void* operator new(size_t i);
	void operator delete(void* p);

};