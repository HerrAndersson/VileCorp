// ------------------------------------------
//		GEOMETRY PASS - PIXEL SHADER
// ------------------------------------------

Texture2D diffuse : register(t0);
Texture2D specular : register(t1);

SamplerState samplerWrap : register(s0);

struct VS_OUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 ambientLight : AMBIENT;
};

struct PS_OUT
{
	float4 diffuse		: SV_Target0;
	float4 normal	    : SV_Target1;
	float4 backbuffer	: SV_Target2;
};


PS_OUT main(VS_OUT input)
{
	PS_OUT output = (PS_OUT)0;

	float4 color = diffuse.Sample(samplerWrap, input.uv);
	output.diffuse = float4(color.xyz * input.ambientLight, 0.0f);
	output.normal = float4(input.normal, 0.0f);
	output.backbuffer = float4(output.diffuse.xyz * input.ambientLight, 0.0f);

	return output;
}