//cbuffer matrixBufferPerObject : register(b0)
//{
//	matrix worldMatrix;
//	float3 colorModifier;
//};
//
//cbuffer matrixBufferPerFrame : register(b1)
//{
//	matrix viewMatrix;
//	matrix projectionMatrix;
//	float3 camPos;
//};

struct vertexInputType
{
	float4 position		: POSITION;
	float2 tex			: TEXCOORD0;
	float3 normal		: NORMAL;
};

struct vertexOutput
{
	float4 position		: SV_POSITION0;
	float2 tex			: TEXCOORD0;
	float3 normal		: NORMAL;
};

vertexOutput main(vertexInputType input)
{
	vertexOutput output;

	output.position = input.position;
	output.tex = input.tex;
	output.normal = input.normal;

	return output;
}