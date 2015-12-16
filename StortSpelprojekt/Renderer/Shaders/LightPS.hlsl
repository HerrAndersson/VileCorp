// ------------------------------------------
//		LIGHT PASS - PIXEL SHADER
// ------------------------------------------

cbuffer matrixBufferLightPassPerFrame : register(b2)
{
	matrix invCamView;
	matrix invCamProj;
};

cbuffer matrixBufferLightPassPerLight : register(b3)
{
	matrix lightView;
	matrix lightProj;
	float3 lightPosition;
	float lightAngle;
	float3 lightDirection;
	float lightIntensity;
	float3 lightColor;
	float lightRange;
}

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

//struct SpotLight
//{
//	float3 position;
//	float3 color;
//	float3 direction;
//	float angle;
//	float intensity;
//	float range;
//};

Texture2D diffuseTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D camDepthMap : register(t2);
Texture2D lightDepthMap : register(t3);

SamplerState samplerWrap : register(s0);
SamplerState samplerClamp : register(s1);

float3 ReconstructWorldFromCamDepth(float2 uv)
{
	// Get the depth value for this pixel
	float z = camDepthMap.Sample(samplerWrap, uv).r;

	// Get x/w and y/w from the viewport position
	float x = uv.x * 2 - 1;											
	float y = (1 - uv.y) * 2 - 1;
	float4 projectedPos = float4(x, y, z, 1.0f);

	float4 worldPos = mul(projectedPos, invCamProj);
	worldPos = mul(worldPos, invCamView);

	return float3(worldPos.xyz / worldPos.w);
}

float4 main(VS_OUT input) : SV_TARGET
{
	PointLight p;
	p.position = float3(3, 1, 6);
	p.color = float3(0.02f, 1.0f, 0.02f);
	p.intensity = 0.1f;

	//SpotLight s;
	//s.position = float3(5, 1, 5);
	//s.color = float3(0, 1, 1);
	//s.direction = float3(1, 0, 0);
	//s.angle = 45;
	//s.intensity = 1.0f;
	//s.range = 15.0f;

	float4 diffuse = diffuseTex.Sample(samplerWrap, input.uv);
	float4 normal = normalTex.Sample(samplerWrap, input.uv);
	normal.w = 0.0f;
	normal = normalize(normal);

	float3 finalColor = diffuse.xyz;

	if (input.uv.x < 0.2f && input.uv.y < 0.2f)
	{
		return pow(camDepthMap.Sample(samplerWrap, input.uv*5.0f).r,10);
	}

	if (input.uv.x > 0.8f && input.uv.y < 0.2f)
	{
		return pow(lightDepthMap.Sample(samplerWrap, input.uv*5.0f).r,10);
	}

	float3 worldPos = ReconstructWorldFromCamDepth(input.uv);
	float3 lightToPixel = lightPosition - worldPos;
	
	float l = length(lightToPixel);
	lightToPixel = normalize(lightToPixel);

	float howMuchLight = dot(lightToPixel, normal.xyz);
	if (howMuchLight > 0.0f  && l < lightRange)
	{
		//Sample and add shadows for the shadow map.
		float4 lightSpacePos = float4(worldPos, 1.0f);
		lightSpacePos = mul(lightSpacePos, lightView);
		lightSpacePos = mul(lightSpacePos, lightProj);

		float2 smTex;
		smTex.x = 0.5f + (lightSpacePos.x / lightSpacePos.w * 0.5f);
		smTex.y = 0.5f - (lightSpacePos.y / lightSpacePos.w * 0.5f);

		float depth = lightSpacePos.z / lightSpacePos.w;

		float epsilon = 0.0001f;
		float dx = 1.0f / 256;

		float s0 = lightDepthMap.Sample(samplerClamp, smTex).r;
		float s1 = lightDepthMap.Sample(samplerClamp, smTex + float2(dx, 0.0f)).r;
		float s2 = lightDepthMap.Sample(samplerClamp, smTex + float2(0.0f, dx)).r;
		float s3 = lightDepthMap.Sample(samplerClamp, smTex + float2(dx, dx)).r;

		float2 texelPos = smTex * 256;
		float2 lerps = frac(texelPos);
		float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

		if (shadowCoeff < depth - epsilon)
		{
			finalColor = saturate(finalColor * shadowCoeff);
		}
		else if (dot(-normalize(lightToPixel), normalize(lightDirection)) > 0.95f)
		{
			finalColor += lightColor * lightIntensity;
			finalColor *= dot(-normalize(lightToPixel), lightDirection);
		}
	}


	//float3 worldPos = ReconstructWorldFromCamDepth(input.uv);
	//float3 lightToPixel = s.position - worldPos;

	//float l = length(lightToPixel);

	//if (dot(normalize(lightToPixel), normal.xyz) > 0)
	//{
	//	float falloff = (p.intensity) / (pow(0.75f, 2)) * (pow(l, 4.4f)); //      dot(lightToPixel, lightToPixel);

	//	finalColor += p.color * p.intensity * falloff;
	//	finalColor *= 1.8f / (l);

	//	//return float4(finalColor, diffuse.a);
	//}

	return saturate(float4(finalColor * float3(0.2f, 0.2f, 0.2f), 1));
}