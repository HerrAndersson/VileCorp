/*----------------------------------------------------------------------------------------------------------------------
| Pixel shader that handles the pixels contained in the geometric volume of the spotlight.							   |
| These pixels will be shaded using additive blending with the temp-backbuffer,										   |
| which is then rendered to the screen in the FXAA stage															   |
----------------------------------------------------------------------------------------------------------------------*/

cbuffer matrixBufferLightPassPerFrame : register(b2)
{
	matrix invCamView;
	matrix invCamProj;
	int screenWidth;
	int screenHeight;
};

cbuffer matrixBufferLightPassPerSpotlight : register(b3)
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
SamplerState samplerPoint : register(s2);

float3 ReconstructWorldFromCamDepth(float2 uv)
{
	// Get the depth value for this pixel, then transform it by inverse camera matrices
	float z = camDepthMap.Sample(samplerWrap, uv).r;

	float x = uv.x * 2 - 1;
	float y = (1 - uv.y) * 2 - 1;

	float4 projectedPos = float4(x, y, z, 1.0f);

	float4 worldPos = mul(projectedPos, invCamProj);
	worldPos = mul(worldPos, invCamView);

	return float3(worldPos.xyz / worldPos.w);
}

//Checks if a given position is inside the cone of the spotlight
bool IsPositionWithinCone(float3 conePos, float3 coneDirection, float3 position, float coneFov, float maxDistance)
{
	float3 differenceVector = position - conePos;
	float l = length(differenceVector);
	differenceVector *= (1 / l);

	if (l > maxDistance)
	{
		return false;
	}

	//return (dot(coneDirection, differenceVector) >= cos(coneFov / 2)); //This is what it should be according to tutorial, but sin eliminates the artifacts
	return (dot(coneDirection,differenceVector) >= sin(coneFov/2));
}

float Get2x2ShadowFactor(float2 smTex, float offset, float2 lerps)
{
	//Sample
	float s0 = lightDepthMap.Sample(samplerPoint, smTex).r;
	float s1 = lightDepthMap.Sample(samplerPoint, smTex + float2(offset, 0.0f)).r;
	float s2 = lightDepthMap.Sample(samplerPoint, smTex + float2(0.0f, offset)).r;
	float s3 = lightDepthMap.Sample(samplerPoint, smTex + float2(offset, offset)).r;

	//Interpolate
	float shadowFactor = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

	return shadowFactor;
}

float Get3x3ShadowFactor(float2 smTex, float offset)
{
	float sum = 0;
	int index = 0;

	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			sum += lightDepthMap.Sample(samplerPoint, smTex + float2(x*offset, y*offset)).r;
		}
	}

	return sum/9;
}

float4 main(VS_OUT input) : SV_TARGET
{
	float lightAngleDiv2 = lightAngle / 2;
	float2 uv = float2((input.pos.x) / screenWidth, (input.pos.y) / screenHeight);

	float4 normal = normalize(float4(normalTex.Sample(samplerWrap, uv).xyz, 0.0f));

	float3 worldPos = ReconstructWorldFromCamDepth(uv);
	float3 pixToLight = lightPosition - worldPos;
	
	float l = length(pixToLight);
	pixToLight = normalize(pixToLight);

	float howMuchLight = dot(pixToLight, normal.xyz);

	float3 nLightDir = normalize(lightDirection);
	float pixToLightAngle = acos(dot(nLightDir, -pixToLight));

	//Inside the cone
	if (howMuchLight > 0.0f && IsPositionWithinCone(lightPosition, lightDirection, worldPos, lightAngle, lightRange))
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
		float dx = 1.0f / shadowMapDimensions;

		float2 texelPos = smTex * shadowMapDimensions;
		float2 lerps = frac(texelPos);
		float shadowFactor = Get2x2ShadowFactor(smTex, dx, lerps);

		float4 diffuse = diffuseTex.Sample(samplerWrap, uv);

		//In light
		if (shadowFactor > depth - epsilon)
		{
			float4 finalColor = float4((diffuse.xyz * lightColor * lightIntensity), 0.5f);

			if (pixToLightAngle > lightAngleDiv2 * 0.72f)
			{
				finalColor.a = 0.15f;
			}
			else if (pixToLightAngle > lightAngleDiv2 * 0.63f)
			{
				finalColor.a = 0.25f;
			}
			else if (pixToLightAngle > lightAngleDiv2 * 0.58f)
			{
				finalColor.a = 0.35f;
			}

			return finalColor;
		}
	}

	//Not in light
	return float4(0, 0, 0, 0);
}