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
	std::vector<DirectX::XMMATRIX> _gridMatrices;
public:
	Grid(ID3D11Device* device, float gridOffset, int gridSize);
	~Grid();
	void* operator new(size_t i);
	void operator delete(void* p);

	std::vector<DirectX::XMMATRIX>* GetGridMatrices();
	ID3D11Buffer* GetLineBuffer();
};