
struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 colorOffset : COLOROFFSET;
};

Texture2D tex : register(t0);
SamplerState ObjSamplerState : register(s0);

float4 main(VS_OUT input) : SV_TARGET
{
	return float4(tex.Sample(ObjSamplerState, input.uv) + float4(input.colorOffset,0));
}