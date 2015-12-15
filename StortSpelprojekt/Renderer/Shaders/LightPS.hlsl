// ------------------------------------------
//		LIGHT PASS - PIXEL SHADER
// ------------------------------------------

cbuffer matrixBufferLightPass : register(b2)
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

struct SpotLight
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

float3 ReconstructWorldFromCamDepth(float2 uv)
{
	// Get the depth value for this pixel
	float z = camDepthMap.Sample(ObjSamplerState, uv).r;				

	// Get x/w and y/w from the viewport position
	float x = uv.x * 2 - 1;											
	float y = (1 - uv.y) * 2 - 1;
	float4 projectedPos = float4(x, y, z, 1.0f);

	float4 worldPos = mul(projectedPos, invertedProjectionMatrix);
	worldPos = mul(worldPos, invertedViewMatrix);

	return float3(worldPos.xyz / worldPos.w);
}

float4 main(VS_OUT input) : SV_TARGET
{
	PointLight p;
	p.position = float3(5, 1, 4);
	p.color = float3(0.02f, 1.0f, 0.02f);
	p.intensity = 0.0001f;

	SpotLight s;
	s.position = float3(2, 1, 2);
	s.color = float3(0, 1.0f, 1);
	s.direction = float3(1, 0, 1);
	s.angle = 30;
	s.intensity = 1.0f;
	s.range = 50.0f;

	float4 diffuse = diffuseTex.Sample(ObjSamplerState, input.uv);
	float4 normal = normalTex.Sample(ObjSamplerState, input.uv);
	normal.w = 0.0f;
	normal = normalize(normal);

	float3 finalAmbient = diffuse.xyz;

	//if (input.uv.x < 0.4f && input.uv.y < 0.4f)
	//{
	//	return pow(camDepthMap.Sample(ObjSamplerState, input.uv*2.5f).r,10);
	//}

	float3 worldPos = ReconstructWorldFromCamDepth(input.uv);
	float3 lightToPixelVec = s.position - worldPos;
	
	float l = length(lightToPixelVec);
	//lightToPixelVec = normalize(lightToPixelVec);

	float howMuchLight = dot(lightToPixelVec, normal.xyz);
	if (howMuchLight > 0.0f)
	{

		if (dot(-normalize(lightToPixelVec), normalize(s.direction)) > 0.95f)
		{
			finalAmbient += s.color * s.intensity;
			finalAmbient *= dot(-normalize(lightToPixelVec), s.direction);
		}
	}

	//if (dot(lightToPixelVec, normal.xyz) < 0)
	//{
		float falloff = (p.intensity) / dot(lightToPixelVec, lightToPixelVec);
		finalAmbient += p.color * p.intensity * falloff;
		finalAmbient *= 1.8f / (l);

		return float4(finalAmbient, diffuse.a);
	//}

	return float4(finalAmbient * float3(0.2f, 0.2f, 0.2f), 1);
}