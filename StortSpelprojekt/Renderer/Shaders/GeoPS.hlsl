// ------------------------------------------
//		GEOMETRY PASS - PIXEL SHADER
// ------------------------------------------

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float4 worldPos : POSITION;
};

struct PS_OUT
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
	float4 position : SV_Target2;
	float4 worldPos : SV_Target3;
};


PS_OUT main(VS_OUT input)
{
	PS_OUT output = (PS_OUT)0;

	output.color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	output.normal = float4(input.normal, 0.0f);
	output.position = input.pos;
	output.worldPos = input.pos;

	return output;
}