/*----------------------------------------------------------------------------------------------------------------------
| Pixel shader used for the rendering of billboarded objects														   |
----------------------------------------------------------------------------------------------------------------------*/

Texture2D textures[4] : register(t0); //The same particle system can use no more than 4 textures to choose from.
SamplerState samplerWrap : register(s0);

struct GS_OUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 worldPos : POS;
};

struct PS_OUT
{
	float4 diffuse		  : SV_Target0;
	float4 normal	      : SV_Target1;
	float4 backbuffercopy : SV_Target2;
};


PS_OUT main(GS_OUT input)
{
	PS_OUT output = (PS_OUT)0;

	//TODO: Randomize which texture to sample from
	float4 color = float4(textures[0].Sample(samplerWrap, input.tex).xyz, 1.0f);

	output.diffuse = color;
	output.normal = float4(input.normal, 1.0f);
	output.backbuffercopy = color; //TODO: apply ambient? / Jonas

	return output;
}