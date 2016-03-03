/*--------------------------------------------------------------------------------------------------------------------
| Pixel shader that is used when FXAA is disabled render the backbuffer-copy to the real backbuffer. 				 |
--------------------------------------------------------------------------------------------------------------------*/

Texture2D backbuffertemp : register(t2);
SamplerState samplerWrap : register(s0);

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main(VS_OUT input) : SV_TARGET
{
	return float4(backbuffertemp.Sample(samplerWrap, input.uv).xyz, 0.0f);
}