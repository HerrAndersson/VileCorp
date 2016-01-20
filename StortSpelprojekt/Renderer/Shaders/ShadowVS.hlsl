cbuffer matrixBufferPerFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer matrixBufferPerObject : register(b1)
{
	matrix worldMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
};

float4 main(VertexInputType input) : SV_POSITION
{
	float4 output = input.position;
	output.w = 1.0f;

	output = mul(output, worldMatrix);
	output = mul(output, viewMatrix);
	output = mul(output, projectionMatrix);

	return output;
}