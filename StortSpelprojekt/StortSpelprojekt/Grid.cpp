#include "Grid.h"

Grid::Grid(ID3D11Device* device, float gridOffset, int gridSize, DirectX::XMFLOAT3 colorOffset)
{
	float lineOffset = (gridOffset * gridSize);
	DirectX::XMFLOAT3 lineVertices[2] =
	{
		{
			lineOffset, 0.0f, 0.0f
		},
		{
			-lineOffset, 0.0f, 0.0f
		},
	};

	_vertexSize = sizeof(DirectX::XMFLOAT3);
	_nrOfPoints = 2;
	_colorOffset = colorOffset;

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = _vertexSize * 2;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = lineVertices;
	HRESULT result = device->CreateBuffer(&bufferDesc, &data, &_lineBuffer);
	if (FAILED(result))
	{
		throw std::runtime_error("Grid: could not create buffer.");
	}

	_gridMatrices = std::vector<DirectX::XMMATRIX>();
	_gridMatrices.reserve(gridSize * 4 + 2);

	for (int i = -gridSize; i <= gridSize; i++)
	{
		_gridMatrices.push_back(DirectX::XMMatrixTranslation(0.5f, 0.01f, i * gridOffset + 0.5f));
		_gridMatrices.push_back((DirectX::XMMatrixRotationY(DirectX::XM_PIDIV2) * DirectX::XMMatrixTranslation(i * gridOffset + 0.5f, 0.01f, 0.5f)));
	}
}

Grid::~Grid()
{
	_lineBuffer->Release();
}

std::vector<DirectX::XMMATRIX>* Grid::GetGridMatrices()
{
	return &_gridMatrices;
}

ID3D11Buffer* Grid::GetLineBuffer()
{
	return _lineBuffer;
}

int Grid::GetVertexSize() const
{
	return _vertexSize;
}

int Grid::GetNrOfPoints() const
{
	return _nrOfPoints;
}

DirectX::XMFLOAT3 Grid::GetColorOffset() const
{
	return _colorOffset;
}

void* Grid::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void Grid::operator delete(void* p)
{
	_mm_free(p);
}
