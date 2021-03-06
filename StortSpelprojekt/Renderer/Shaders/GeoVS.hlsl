/*----------------------------------------------------------------------------------------------------------------------
| Vertex shader used to transform the static geometry to the screenspace for rendering.								   |
| Also passes through color offset and ambient light																   |
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
	float3 colorOffset;
};

struct VS_IN
{
	float3 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
};

struct VS_OUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 ambientLight : AMBIENT;
	float3 colorOffset  : COLOROFFSET;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 pos = mul(float4(input.pos, 1.0f), worldMatrix);
	pos = mul(pos, viewMatrix);
	pos = mul(pos, projectionMatrix);

	output.pos = pos;
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.uv = input.uv;
	output.ambientLight = ambientLight;
	output.colorOffset = colorOffset;
	
	return output;
}