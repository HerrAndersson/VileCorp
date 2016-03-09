/*----------------------------------------------------------------------------------------------------------------------
| Vertex shader that is used to render a flat quad to the screen, offset with the quads position.					   |
----------------------------------------------------------------------------------------------------------------------*/

cbuffer TextureModelMatrix : register(b1)
{
	matrix model;
	float4 colorOffset;
	int hasTexture;
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
	float4 colorOffset : COLOROFFSET;
	int hasTexture : HASTEXTURE;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 pos = mul(float4(input.pos, 1.0f), model);

	output.pos = pos;
	output.uv = input.uv;
	output.colorOffset = colorOffset;
	output.hasTexture = hasTexture;

	return output;
}