/*----------------------------------------------------------------------------------------------------------------------
| Vertex that is used to transform the grid (each linelist object) to the screen. 									   |
----------------------------------------------------------------------------------------------------------------------*/

cbuffer matrixBufferPerFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer matrixBufferPerObject : register(b1)
{
	matrix worldMatrix;
	float3 colorOffset;
};

struct VS_IN
{
	float3 pos		: POSITION;
};

struct VS_OUT
{
	float4 pos		: SV_POSITION;
	float3 colorOffset : COLOROFFSET;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 pos = mul(float4(input.pos, 1.0f), worldMatrix);
	pos = mul(pos, viewMatrix);
	pos = mul(pos, projectionMatrix);

	output.pos = pos;
	output.colorOffset = colorOffset;

	return output;
}