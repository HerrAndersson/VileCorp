/* ------------------------------------------
		FXAA PASS - PIXEL SHADER
------------------------------------------*/

Texture2D diffuse : register(t0);

SamplerState samplerWrap : register(s0);

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{
	float3 testOut = diffuse.Sample(samplerWrap, input.uv);

	return float4(testOut, 0.0f);
}