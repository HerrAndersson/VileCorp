/*--------------------------------------------------------------------------------------------------------------------
| Vertex shader that simply passes the position and the texture coodinates to the pixel shader 			             |
--------------------------------------------------------------------------------------------------------------------*/

struct VS_IN
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = float4(input.pos, 1.0f);
	output.uv = input.uv;

	return output;
}