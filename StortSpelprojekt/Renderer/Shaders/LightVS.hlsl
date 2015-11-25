/* ------------------------------------------
		LIGHT PASS - VERTEX SHADER
   ------------------------------------------*/


struct VS_IN
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
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