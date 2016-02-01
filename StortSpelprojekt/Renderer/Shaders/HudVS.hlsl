/* ------------------------------------------
	TEXTURE - VERTEX SHADER
------------------------------------------*/

cbuffer TextureModelMatrix : register(b1)
{
	matrix	model;
};

struct VS_IN
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD0;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 pos = mul(float4(input.pos, 1.0f), model);

	output.pos = pos;
	output.uv = input.uv;

	return output;
}