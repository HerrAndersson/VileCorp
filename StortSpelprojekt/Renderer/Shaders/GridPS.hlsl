// ------------------------------------------
//		GRID PASS - PIXEL SHADER
// ------------------------------------------

SamplerState samplerWrap : register(s0);


struct VS_OUT
{
	float4 pos		: SV_POSITION;
	float4 color : COLOR;
	float4 worldPos : POSITION;
};

float4 main(VS_OUT input) : SV_TARGET
{
	input.color = float4(0.0, 0.0, 0.0, 1.0);

	return input.color;
}