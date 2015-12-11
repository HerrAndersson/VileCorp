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

struct DirectionalLight
{
	float3 position;
	float3 color;
	float3 direction;
	float angle;
	float intensity;
	float range;
};

Texture2D diffuseTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D position : register(t2);
Texture2D worldPosTex : register(t3);
Texture2D depthMap : register(t4);

SamplerState ObjSamplerState : register(s0);

float4 main(VS_OUT input) : SV_TARGET
{
	//PointLight p;
	//p.position = float3(5, 1, 4);
	//p.color = float3(0.02f, 1.0f, 0.02f);
	//p.intensity = 0.01f;

	float4 diffuse = diffuseTex.Sample(ObjSamplerState, input.uv);
	float4 normal = normalTex.Sample(ObjSamplerState, input.uv);
	float4 worldPos = worldPosTex.Sample(ObjSamplerState, input.uv);
	worldPos.w = 1.0f;

	normal.w = 0.0f;
	normal = normalize(normal);

	float3 finalAmbient = diffuse.xyz;// *0.2f;

	//DirectionalLight d;
	//d.position = float3(2, 2, 2);
	//d.color = float3(0, 1.0f, 1);
	//d.direction = float3(1, 0, 1);
	//d.angle = 30;
	//d.intensity = 0.3f;
	//d.range = 5.0f;


	//float3 lightToPixelVec = d.position - worldPos.xyz;
	//float l = length(lightToPixelVec);

	//float howMuchLight = dot(lightToPixelVec, normal);

	//if (howMuchLight > 0.0f)
	//{

	//	if (dot(-normalize(lightToPixelVec), normalize(d.direction)) > 0.95f && length(lightToPixelVec) < d.range + 0.001f)
	//	{
	//		finalAmbient += d.color * d.intensity;
	//		finalAmbient *= dot(-normalize(lightToPixelVec), d.direction);
	//	}
	//}










	//float3 lv = worldPos.xyz - p.position;
	//float len = length(lv);

	/*float depth1 = depthMap.Sample(ObjSamplerState, input.uv).r;
	float depth2;
	bool shadowed = false;
	for (int i = 0; i < 100; i++)
	{
		if (len < 0)
		{
			depth2 = depthMap.Sample(ObjSamplerState, input.uv + float2(lv.x / 100, lv.y / 100)).r;
		}
		else
		{
			depth2 = depthMap.Sample(ObjSamplerState, input.uv - float2(lv.x / 100, lv.y / 100)).r;
		}

		if (depth1 + 0.00101f < depth2)
		{
			shadowed = true;
		}
		depth1 = depth2;
	}
*/
	//if (dot(lv, normal.xyz) < 0)
	//{
	//	float falloff = (p.intensity) / dot(lv, lv);
	//	finalAmbient += p.color * p.intensity * falloff;
	//	//finalAmbient *= falloff;
	//	finalAmbient *= 1.8f/(len);
	//}

	return float4(finalAmbient, diffuse.a);
}