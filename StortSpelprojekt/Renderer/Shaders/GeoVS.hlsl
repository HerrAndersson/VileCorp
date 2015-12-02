/* ------------------------------------------
		GEOMETRY PASS - VERTEX SHADER
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
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
};

struct VS_OUT
{
	float4 pos		: SV_POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
	float4 worldPos : POSITION;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 pos = mul(float4(input.pos, 1.0f), worldMatrix);
	output.worldPos = pos;
	//pos = mul(viewMatrix, pos);
	//pos = mul(projectionMatrix, pos);
	pos = mul(viewMatrix, pos);
	pos = mul(projectionMatrix, pos);

	output.pos = pos;



	output.normal = input.normal;
	output.uv = input.uv;
	

	return output;
}