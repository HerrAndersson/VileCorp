/*--------------------------------------------------------------------------------------------------------------------
| Animation Vertex shader used for generate the depth buffer for objects that should cast shadows.					 |
| Binds null-rendertarget and a depth stencil view																     |
--------------------------------------------------------------------------------------------------------------------*/

cbuffer matrixBufferPerFrame : register(b4)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer matrixBufferPerObject : register(b1)
{
	matrix worldMatrix;
};

cbuffer matrixBufferPerObjectObject : register(b5)
{
	matrix bones[50];
};

struct VertexInputType
{
	uint4 boneIndex		: BONEINDEX;
	float3 pos			: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float4 boneWeight	: BONEWEIGHT;
};

float4 main(VertexInputType input) : SV_POSITION
{
	float4 inputPos = float4(input.pos, 1.0f);

	// Calculating the new position for every vertex based on the bones matrices
	float4 animPos = mul(inputPos, bones[input.boneIndex.y]) * input.boneWeight.y;
	animPos += mul(inputPos, bones[input.boneIndex.x]) * input.boneWeight.x;
	animPos += mul(inputPos, bones[input.boneIndex.z]) * input.boneWeight.z;
	animPos += mul(inputPos, bones[input.boneIndex.w]) * input.boneWeight.w;

	animPos = mul(animPos, worldMatrix);
	animPos = mul(animPos, viewMatrix);
	animPos = mul(animPos, projectionMatrix);

	return animPos;
}