// ------------------------------------------
//		LIGHT PASS - PIXEL SHADER
// ------------------------------------------

cbuffer matrixBufferLightPassPerFrame : register(b2)
{
	matrix invCamView;
	matrix invCamProj;
	int screenWidth;
	int screenHeight;
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
	int shadowMapDimensions;
}

struct VS_OUT
{
	float4 pos : SV_POSITION;
};

Texture2D diffuseTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D camDepthMap : register(t3);
Texture2D lightDepthMap : register(t4);

SamplerState samplerWrap : register(s0);
SamplerState samplerClamp : register(s1);

float3 ReconstructWorldFromCamDepth(float2 uv)
{
	// Get the depth value for this pixel
	float z = camDepthMap.Sample(samplerWrap, uv).r;

	float x = uv.x * 2 - 1;
	float y = (1 - uv.y) * 2 - 1;

	float4 projectedPos = float4(x, y, z, 1.0f);

	float4 worldPos = mul(projectedPos, invCamProj);
	worldPos = mul(worldPos, invCamView);

	return float3(worldPos.xyz / worldPos.w);
}

float4 main(VS_OUT input) : SV_TARGET
{
	float lightAngleDiv2 = lightAngle / 2;
	float2 uv = float2((input.pos.x) / screenWidth, (input.pos.y) / screenHeight);

	float4 normal = normalize(float4(normalTex.Sample(samplerWrap, uv).xyz, 0.0f));

	//if (uv.x < 0.75f && uv.y < 0.75f && uv.x > 0.25f && uv.y > 0.25f)
	//{
	//	return pow(camDepthMap.Sample(samplerWrap, uv).r,10);
	//}
	//if (uv.x > 0.8f && uv.y < 0.2f)
	//{
	//	return pow(lightDepthMap.Sample(samplerWrap, uv*5.0f).r,10);
	//}

	float3 worldPos = ReconstructWorldFromCamDepth(uv);
	float3 pixToLight = lightPosition - worldPos;
	
	float l = length(pixToLight);
	pixToLight = normalize(pixToLight);

	float howMuchLight = dot(pixToLight, normal.xyz);

	float3 nLightDir = normalize(lightDirection);
	float pixToLightAngle = acos(dot(nLightDir, -pixToLight));
	
	//Inside of the volume
	if (howMuchLight > 0.0f && pixToLightAngle <= lightAngleDiv2)
	{
		//Sample and add shadows for the shadow map.
		float4 lightSpacePos = float4(worldPos, 1.0f);
		lightSpacePos = mul(lightSpacePos, lightView);
		lightSpacePos = mul(lightSpacePos, lightProj);

		float2 smTex;
		smTex.x = 0.5f + (lightSpacePos.x / lightSpacePos.w * 0.5f);
		smTex.y = 0.5f - (lightSpacePos.y / lightSpacePos.w * 0.5f);

		float depth = lightSpacePos.z / lightSpacePos.w;

		float epsilon = 0.00001f;
		float dx = 1.0f / shadowMapDimensions;

		float s0 = lightDepthMap.Sample(samplerClamp, smTex).r;
		float s1 = lightDepthMap.Sample(samplerClamp, smTex + float2(dx, 0.0f)).r;
		float s2 = lightDepthMap.Sample(samplerClamp, smTex + float2(0.0f, dx)).r;
		float s3 = lightDepthMap.Sample(samplerClamp, smTex + float2(dx, dx)).r;

		float2 texelPos = smTex * shadowMapDimensions;
		float2 lerps = frac(texelPos);
		float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

		float4 diffuse = diffuseTex.Sample(samplerWrap, uv);

		//In light
		if (shadowCoeff > depth - epsilon)
		{
			float4 finalColor = float4((diffuse.xyz * lightColor * lightIntensity), 0.5f);

			if (pixToLightAngle > lightAngleDiv2 * 0.85)
			{
				finalColor *= float4(0.1f, 0.1f, 0.1f, 0.1f);
			}
			else if (pixToLightAngle > lightAngleDiv2 * 0.75)
			{
				finalColor *= float4(0.35f, 0.35f, 0.35f, 0.35f);
			}
			else if (pixToLightAngle > lightAngleDiv2 * 0.65)
			{
				finalColor *= float4(0.7f, 0.7f, 0.7f, 0.65f);
			}

			return finalColor;
		}
		else
		{
			//In shadow
			//Test how far away the lit parts are to generate the "toon" falloff
			/*return float4(diffuse.xyz * float3(0.1, 0.1, 0.1), 0.5f);*/
		}
	}

	//Not in light
	return float4(0, 0, 0, 0);
}