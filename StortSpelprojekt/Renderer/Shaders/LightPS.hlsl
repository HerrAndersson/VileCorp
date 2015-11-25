// ------------------------------------------
//		LIGHT PASS - PIXEL SHADER
// ------------------------------------------

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D diffuseTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D position : register(t2);
Texture2D worldPosTex : register(t3);

SamplerState ObjSamplerState : register(s0);

float4 main(VS_OUT input) : SV_TARGET
{
	float4 diffuse = diffuseTex.Sample(ObjSamplerState, input.uv);
	float4 normal = normalTex.Sample(ObjSamplerState, input.uv);

	float4 worldPos = worldPosTex.Sample(ObjSamplerState, input.uv);
	worldPos.w = 1.0f;

	normal.w = 0.f;
	normal = normalize(normal);

	float3 finalColor = { 0.0f, 0.0f, 0.0f };
	float3 finalAmbient = diffuse.xyz;

	return float4(finalColor, diffuse.a);
}