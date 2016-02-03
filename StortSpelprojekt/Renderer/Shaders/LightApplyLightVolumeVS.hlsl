/*----------------------------------------------------------------------------------------------------------------------
| Vertex shader used to "render" the geometric volumes of spotlights and pointlights.								   |
| These will be invisible themselves, but will shade the pixels contained in the volumes.					           |
----------------------------------------------------------------------------------------------------------------------*/

cbuffer matrixBufferPerFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 ambientLight;
};

cbuffer matrixBufferPerObject : register(b1)
{
	matrix worldMatrix;
};

struct VS_IN
{
	float3 pos : POSITION;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 pos = mul(float4(input.pos, 1.0f), worldMatrix);
	pos = mul(pos, viewMatrix);
	pos = mul(pos, projectionMatrix);

	output.pos = pos;

	return output;
}