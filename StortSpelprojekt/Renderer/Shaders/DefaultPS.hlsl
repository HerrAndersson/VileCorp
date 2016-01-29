Texture2D diffuse : register(t0);
Texture2D specular : register(t1);

struct pixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 main(pixelInputType input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	//float4 finalColor = AssetTexture.Sample(sampleStateClamp, input.tex);

	return float4(1, 0, 0, 0);

}