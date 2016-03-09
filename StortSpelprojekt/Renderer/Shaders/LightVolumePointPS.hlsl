/*----------------------------------------------------------------------------------------------------------------------
| Pixel shader that handles the pixels contained in the geometric volume of the pointlight.							   |
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

cbuffer matrixBufferLightPassPerPointlight : register(b3)
{
	float3 lightPosition;
	float lightIntensity;
	float3 lightColor;
	float lightRange;
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
	float lightRangeDiv2 = lightRange / 2;
	float2 uv = float2((input.pos.x) / screenWidth, (input.pos.y) / screenHeight);

	float4 normal = normalize(float4(normalTex.Sample(samplerWrap, uv).xyz, 0.0f));

	float3 worldPos = ReconstructWorldFromCamDepth(uv);
	float3 pixToLight = lightPosition - worldPos;

	float len = length(pixToLight);
	pixToLight = normalize(pixToLight);

	float howMuchLight = dot(pixToLight, normal.xyz);

	//Inside of the volume
	if (howMuchLight > 0.0f && len < lightRangeDiv2)
	{
		float4 diffuse = float4(diffuseTex.Sample(samplerWrap, uv).xyz, 0.0f);
		float4 finalColor = float4(((diffuse.xyz + lightColor) * lightIntensity), 0.5f);

		//calculate basic attenuation
		float r = lightRange/4;
		float3 L = lightPosition - worldPos;
		float d = max(len - r, 0);
		L /= len;

		float denom = d / r + 1;
		float attenuation = 1 / (denom*denom*denom);

		attenuation = (attenuation - 0.005f) / (1 - 0.005f);
		attenuation = max(attenuation, 0);

		return finalColor * max(howMuchLight, 0) * attenuation;
	}

	//Not in light
	return float4(0, 0, 0, 0);
}