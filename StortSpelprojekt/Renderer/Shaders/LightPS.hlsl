// ------------------------------------------
//		LIGHT PASS - PIXEL SHADER
// ------------------------------------------

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

struct PointLight
{
	float3 position;
	float3 color;
	float intensity;
};

Texture2D diffuseTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D position : register(t2);
Texture2D worldPosTex : register(t3);

SamplerState ObjSamplerState : register(s0);

float4 main(VS_OUT input) : SV_TARGET
{

	PointLight p;
	p.position = float3(3, 1, 4);
	p.color = float3(0, 0.8f, 0);
	p.intensity = 1.0f;

	float4 diffuse = diffuseTex.Sample(ObjSamplerState, input.uv);
	float4 normal = normalTex.Sample(ObjSamplerState, input.uv);

	float4 worldPos = worldPosTex.Sample(ObjSamplerState, input.uv);
	worldPos.w = 1.0f;

	normal.w = 0.f;
	normal = normalize(normal);

	float3 finalAmbient = diffuse.xyz * 0.2f;

	float3 lv = worldPos.xyz - p.position;
	float len = length(lv);

	if (dot(lv, normal.xyz) < 0)
	{
		float falloff = (0.5f) / dot(lv, lv);
		finalAmbient += p.color * p.intensity * falloff;
		//finalAmbient *= falloff;
		finalAmbient *= 1.8f/len;
	}

	return float4(finalAmbient, diffuse.a);
}