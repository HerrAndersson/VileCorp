// ------------------------------------------
//		GRID PASS - PIXEL SHADER
// ------------------------------------------

struct VS_OUT
{
	float4 pos		: SV_POSITION;
	float3 colorOffset : COLOROFFSET;
};

float4 main(VS_OUT input) : SV_TARGET2 //Output to backbuffer only
{
	return float4(input.colorOffset, 1.0f);
}