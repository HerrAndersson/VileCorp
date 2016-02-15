/*----------------------------------------------------------------------------------------------------------------------
| Pixel shader that samples and renders a texture.																	   |
----------------------------------------------------------------------------------------------------------------------*/

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 colorOffset : COLOROFFSET;
};

Texture2D tex : register(t0);
SamplerState samplerClamp : register(s1);

float4 main(VS_OUT input) : SV_TARGET
{
	float4 sampledTexture = tex.Sample(samplerClamp, input.uv);
	return float4(sampledTexture.xyz + input.colorOffset.xyz, sampledTexture.a * input.colorOffset.a);;
}