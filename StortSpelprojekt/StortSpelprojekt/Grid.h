#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <stdexcept>

class Grid
{
private:

	ID3D11Buffer* _lineBuffer;

	float _gridOffset;
	int _gridSize;
	int _vertexSize;
	int _nrOfPoints;

	DirectX::XMFLOAT3 _colorOffset;

	std::vector<DirectX::XMMATRIX> _gridMatrices;

public:

	Grid(ID3D11Device* device, float gridOffset, int gridSize, DirectX::XMFLOAT3 colorOffset = DirectX::XMFLOAT3(0,0,0));
	~Grid();

	std::vector<DirectX::XMMATRIX>* GetGridMatrices();
	ID3D11Buffer* GetLineBuffer();

	int GetVertexSize() const;
	int GetNrOfPoints() const;
	DirectX::XMFLOAT3 GetColorOffset() const;

	void* operator new(size_t i);
	void operator delete(void* p);

};