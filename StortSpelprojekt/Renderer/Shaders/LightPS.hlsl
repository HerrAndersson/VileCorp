// ------------------------------------------
//		LIGHT PASS - PIXEL SHADER
// ------------------------------------------

cbuffer matrixBufferLightPass : register(b0)
{
	matrix invertedViewMatrix;
	matrix invertedProjectionMatrix;
};

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
Texture2D camDepthMap : register(t2);
Texture2D lightDepthMap : register(t3);

SamplerState ObjSamplerState : register(s0);

float3 PositionFromDepth(float2 uv)
{
	float z = camDepthMap.Sample(ObjSamplerState, uv);				// Get the depth value for this pixel
	
	// Get x/w and y/w from the viewport position
	float x = uv.x * 2 - 1;
	float y = (1 - uv.y) * 2 - 1;
	float4 projectedPos = float4(x, y, z, 1.0f);

	float4 position = mul(projectedPos, invertedProjectionMatrix);	// Transform by the inverse projection matrix
	return mul(position, invertedViewMatrix).xyz;
}

float4 main(VS_OUT input) : SV_TARGET
{
	//PointLight p;
	//p.position = float3(5, 1, 4);
	//p.color = float3(0.02f, 1.0f, 0.02f);
	//p.intensity = 0.01f;

	float4 diffuse = diffuseTex.Sample(ObjSamplerState, input.uv);
	float4 normal = normalTex.Sample(ObjSamplerState, input.uv);
	normal.w = 0.0f;
	normal = normalize(normal);

	if (input.uv.x < 0.4f && input.uv.y < 0.4f)
	{
		return pow(lightDepthMap.Sample(ObjSamplerState, input.uv*2.5f).r,10);
	}

	float3 finalAmbient = diffuse.xyz;// *0.2f;

	DirectionalLight d;
	d.position = float3(2, 1, 2);
	d.color = float3(0, 1.0f, 1);
	d.direction = float3(1, 0, 1);
	d.angle = 30;
	d.intensity = 0.3f;
	d.range = 5.0f;

	//To apply shadows:
	float3 worldPos = PositionFromDepth(input.uv);

	//float2 smTex;
	//smTex.x = 0.5f + (lightSpacePos.x / lightSpacePos.w * 0.5f);
	//smTex.y = 0.5f - (lightSpacePos.y / lightSpacePos.w * 0.5f);
	//
	//float depth = lightSpacePos.z / lightSpacePos.w;
	//



	float3 lightToPixelVec = d.position - worldPos.xyz;
	
	float l = length(lightToPixelVec);

	float howMuchLight = dot(lightToPixelVec, normal.xyz);

	if (howMuchLight > 0.0f)
	{

		if (dot(-normalize(lightToPixelVec), normalize(d.direction)) > 0.95f && length(lightToPixelVec) < d.range + 0.001f)
		{
			finalAmbient += d.color * d.intensity;
			finalAmbient *= dot(-normalize(lightToPixelVec), d.direction);
		}
	}

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
	}*/

	//if (dot(lv, normal.xyz) < 0)
	//{
	//	float falloff = (p.intensity) / dot(lv, lv);
	//	finalAmbient += p.color * p.intensity * falloff;
	//	//finalAmbient *= falloff;
	//	finalAmbient *= 1.8f / (len);

	//	return float4(finalAmbient, diffuse.a);
	//}

	return float4(finalAmbient * float3(0.2f, 0.2f, 0.2f), 1);
}