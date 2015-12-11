/* ------------------------------------------
GRID PASS - VERTEX SHADER
--------------------------------------------*/

cbuffer matrixBufferPerFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer matrixBufferPerObject : register(b1)
{
	matrix worldMatrix;
};

struct VS_IN
{
	float3 pos		: POSITION;
	float4 color : COLOR;
};

struct VS_OUT
{
	float4 pos		: SV_POSITION;
	float4 color : COLOR;
	float4 worldPos : POSITION;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 pos = mul(float4(input.pos, 1.0f), worldMatrix);
	output.worldPos = pos;
	pos = mul(pos, viewMatrix);
	pos = mul(pos, projectionMatrix);

	output.pos = pos;
	output.color = input.color;

	return output;
}