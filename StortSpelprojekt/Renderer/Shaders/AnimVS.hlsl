/* ------------------------------------------
Animation PASS - VERTEX SHADER
--------------------------------------------*/

cbuffer matrixBufferPerFrame : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer matrixBufferPerSkinnedObject : register(b1)
{
	matrix worldMatrix;
	matrix bones[30];
};

struct VS_IN
{
	uint4 boneIndex		: BONEINDEX;
	float3 pos			: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float4 boneWeight	: BONEWEIGHT;
};

struct VS_OUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float4 worldPos		: POSITION;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	
	float4 inputPos = float4(input.pos, 1.0f);
	float4 animPos;
	float4 animNormal = float4(input.normal, 0.0f);

	// Calculating the new position for every vertex based on the bones matrices
	animPos = mul(inputPos, bones[input.boneIndex.y]) * input.boneWeight.y;
	animPos += mul(inputPos, bones[input.boneIndex.x]) * input.boneWeight.x;
	animPos += mul(inputPos, bones[input.boneIndex.z]) * input.boneWeight.z;
	animPos += mul(inputPos, bones[input.boneIndex.w]) * input.boneWeight.w;

	animNormal = mul(animNormal, bones[input.boneIndex.x]) * input.boneWeight.x;
	animNormal += mul(animNormal, bones[input.boneIndex.y]) * input.boneWeight.y;
	animNormal += mul(animNormal, bones[input.boneIndex.z]) * input.boneWeight.z;
	animNormal += mul(animNormal, bones[input.boneIndex.w]) * input.boneWeight.w;

	animPos = mul(animPos, worldMatrix);
	output.worldPos = animPos;
	animPos = mul(animPos, viewMatrix);
	animPos = mul(animPos, projectionMatrix);

	output.pos = animPos;
	output.normal = animNormal.xyz;
	output.uv = input.uv;

	return output;
}