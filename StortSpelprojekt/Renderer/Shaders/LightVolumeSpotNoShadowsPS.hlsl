/*----------------------------------------------------------------------------------------------------------------------
| Pixel shader that handles the pixels contained in the geometric volume of the spotlight. No Shadow map.		       |
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

	return (dot(coneDirection, differenceVector) >= sin(coneFov / 2));
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
		float4 diffuse = diffuseTex.Sample(samplerWrap, uv);

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
	
	//Not in light
	return float4(0, 0, 0, 0);
}