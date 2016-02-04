#include "Grid.h"

Grid::Grid(ID3D11Device* device, float gridOffset, int gridSizeX, int gridSizeY, DirectX::XMFLOAT3 colorOffset)
{
	_vertexSize = sizeof(DirectX::XMFLOAT3);
	_nrOfPoints = 2;
	_colorOffset = colorOffset;
	_gridSizeX = gridSizeX;
	_gridSizeY = gridSizeY;
	_gridOffset = gridOffset;

	DirectX::XMFLOAT3 lineVertices[2] = { { 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } };

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

	CreateGrid();
}

Grid::~Grid()
{
	_lineBuffer->Release();
}

void Grid::CreateGrid()
{
	_gridMatrices = std::vector<DirectX::XMMATRIX>();
	_gridMatrices.reserve(_gridSizeY * 4 + 2);

	for (int i = -1; i <= _gridSizeY; i++)
	{
		_gridMatrices.push_back(DirectX::XMMatrixScaling(_gridSizeX + 1.0f, 1.0f, 1.0f) * DirectX::XMMatrixTranslation(-0.5f, 0.01f, i * _gridOffset + 0.5f));
	}
	for (int i = -1; i <= _gridSizeX; i++)
	{
		_gridMatrices.push_back(DirectX::XMMatrixScaling(-_gridSizeY - 1.0f, 1.0f, 1.0f) * (DirectX::XMMatrixRotationY(DirectX::XM_PIDIV2) * DirectX::XMMatrixTranslation(i * _gridOffset + 0.5f, 0.01f, -0.5f)));
	}
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

void Grid::ChangeGridSize(int gridSizeX, int gridSizeY, int gridOffset)
{
	_gridSizeX = gridSizeX;
	_gridSizeY = gridSizeY;
	_gridOffset = (float)gridOffset;

	CreateGrid();
}

void* Grid::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void Grid::operator delete(void* p)
{
	_mm_free(p);
}
