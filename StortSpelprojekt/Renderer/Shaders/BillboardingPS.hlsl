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
	float4 color : COLOROFFSET;
	float3 ambientLight : AMBIENT;
	float textureNumber : TEXNUM;
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

	float4 color = float4(0, 0, 0, 0); 
	float texNum = input.textureNumber;

	if (texNum < 1)
	{
		color = textures[0].Sample(samplerWrap, input.tex);
	}
	else if (texNum < 2)
	{
		color = textures[1].Sample(samplerWrap, input.tex);
	}
	else if (texNum < 3)
	{
		color = textures[2].Sample(samplerWrap, input.tex);
	}
	else if (texNum < 4)
	{
		color = textures[3].Sample(samplerWrap, input.tex);
	}

	if (color.a < 1.0f)
	{
		discard;
	}

	output.diffuse = color;
	output.normal = float4(input.normal, 1.0f);
	output.backbuffercopy = color;

	return output;
}