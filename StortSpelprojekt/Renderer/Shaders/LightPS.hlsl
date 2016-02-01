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
	return saturate(float4(diffuse.xyz, 1));
}