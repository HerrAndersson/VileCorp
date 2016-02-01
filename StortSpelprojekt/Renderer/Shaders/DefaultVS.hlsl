cbuffer matrixBufferPerFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer matrixBufferPerObject : register(b1)
{
	matrix worldMatrix;
};

struct vertexInputType
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct vertexOutput
{
	float4 pos			: SV_POSITION0;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD0;
};

vertexOutput main(vertexInputType input)
{
	vertexOutput output;

	output.pos = float4(input.pos, 1.0f);
	output.uv = input.uv;
	output.normal = input.normal;

	return output;
}