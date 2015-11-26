/* ------------------------------------------
		GEOMETRY PASS - VERTEX SHADER
--------------------------------------------*/

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
	float3 normal : NORMAL;
	float4 worldPos : POSITION;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	float4 pos = float4(input.pos, 1.0f);

	output.pos = pos;
	output.uv = input.uv;
	output.normal = input.normal;
	output.worldPos = float4(input.pos, 1.0f);

	return output;
}