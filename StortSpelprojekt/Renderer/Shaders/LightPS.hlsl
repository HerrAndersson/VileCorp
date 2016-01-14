Texture2D diffuseTex : register(t0);
SamplerState samplerWrap : register(s0);

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{
	float4 diffuse = diffuseTex.Sample(samplerWrap, input.uv);
	float shadowCoeff = diffuse.a;

	diffuse *= shadowCoeff;

	return saturate(float4(diffuse.xyz * float3(0.2f, 0.2f, 0.2f), 1));
}