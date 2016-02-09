/*----------------------------------------------------------------------------------------------------------------------
| Pixel shader used for the rendering of billboarded objects														   |
----------------------------------------------------------------------------------------------------------------------*/

Texture2D txDiffuse : register(t0);
SamplerState samplerWrap : register(s0);

struct GS_OUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 worldPos : POS;
};

struct PS_OUT
{
	float4 color : SV_Target0;
	float3 normal : SV_Target1;
	float4 worldPos : SV_Target2;
};


PS_OUT main(GS_OUT input) : SV_Target
{
	PS_OUT output = (PS_OUT)0;

	float3 s = txDiffuse.Sample(sampAni, input.tex).xyz;

	output.color = float4(s, 1.0f);
	output.normal = input.normal;
	output.worldPos = input.worldPos;

	return output;
}